from dash import Dash, dcc, html, Input, Output, dash_table
import dash_bootstrap_components as dbc
import plotly.graph_objects as go
import pandas as pd
import numpy as np
import math
import requests
from decimal import Decimal

app = Dash(__name__, external_stylesheets=[dbc.themes.CYBORG])

# Add custom CSS for dropdowns
app.index_string = '''
<!DOCTYPE html>
<html>
    <head>
        {%metas%}
        <title>{%title%}</title>
        {%favicon%}
        {%css%}
        <style>
            .dropdown-dark .Select-control {
                background-color: #0c0c0c !important;
                border-color: #333 !important;
            }
            .dropdown-dark .Select-menu-outer {
                background-color: #0c0c0c !important;
                border-color: #333 !important;
            }
            .dropdown-dark .Select-value-label {
                color: #fff !important;
            }
            .dropdown-dark .Select-option {
                background-color: #0c0c0c !important;
                color: #fff !important;
            }
            .dropdown-dark .Select-option:hover {
                background-color: #1a1a1a !important;
            }
            .Select-arrow {
                border-color: #666 transparent transparent !important;
            }
        </style>
    </head>
    <body>
        {%app_entry%}
        <footer>
            {%config%}
            {%scripts%}
            {%renderer%}
        </footer>
    </body>
</html>
'''

def aggregate_orderbook_levels(levels_df, side, agg_level = Decimal('0.1')):
    if side == 'bid':
        right = False
        lambda_func =  lambda x: x.left
    elif side == 'ask':
        right = True
        lambda_func = lambda x: x.right

    min_level = math.floor(Decimal(min(levels_df['price'])) / agg_level - 1) * agg_level
    max_level = math.ceil(Decimal(max(levels_df['price'])) / agg_level + 1) * agg_level

    level_bounds = [float(min_level + agg_level * i)
        for i in range(int((max_level - min_level) / agg_level) + 1)
    ]

    levels_df["bin"] = pd.cut(
        levels_df['price'],
        bins=level_bounds,
        right=right,
        precision=10
    )

    levels_df = levels_df.groupby("bin").agg(
        quantity = ("quantity", "sum")
    ).reset_index()
    
    levels_df['price'] = levels_df['bin'].apply(lambda_func)

    levels_df = levels_df[levels_df['quantity'] > 0]
    levels_df = levels_df[['price', 'quantity']]

    return levels_df

def fetch_orderbook_data(symbol: str, limit: int):
    url = "https://api.binance.com/api/v3/depth"
    params = {
        "symbol": symbol.upper(),  # Ensure symbol is uppercase for Binance API
        "limit": limit
    }

    try:
        response = requests.get(url, params=params)
        response.raise_for_status()
        data = response.json()

        # Process asks
        asks = pd.DataFrame(data['asks'], columns=['price', 'quantity'], dtype=float)
        asks['side'] = 'ask'
        asks = asks.sort_values(by='price', ascending=True)  # Ascending for asks

        # Process bids
        bids = pd.DataFrame(data['bids'], columns=['price', 'quantity'], dtype=float)
        bids['side'] = 'bid'
        bids = bids.sort_values(by='price', ascending=False)  # Descending for bids

        # Process mid_price
        mid_price = (bids['price'].iloc[0] + asks['price'].iloc[-1]) / 2

        return asks, bids, mid_price

    except requests.exceptions.RequestException as e:
        print(f"Error fetching orderbook: {e}")
        # Return empty DataFrames with correct columns if there's an error
        empty_df = pd.DataFrame(columns=['price', 'quantity', 'side'])
        return empty_df, empty_df

# Dash DataTable properties
def make_table(df: pd.DataFrame, color: str):
    """
    Create styled DataTable for asks (red) or bids (green)
    """
    df = df.round(3)
    
    # Calculate cumulative quantity for depth visualization
    df['cumulative_quantity'] = df['quantity'].cumsum()
    max_cumul_qty = df['cumulative_quantity'].max() if not df.empty else 1
    
    style_data_conditional = []
    for i, row in df.iterrows():
        # Use cumulative quantity for depth effect
        pct = row['cumulative_quantity'] / max_cumul_qty
        alpha = 0.1 + 0.4 * pct  # opacity by depth
        bg_color = f'rgba(255,0,0,{alpha})' if color == 'red' else f'rgba(0,255,0,{alpha})'
        text_color = '#ff6b6b' if color == 'red' else '#00ff99'
        style_data_conditional += [
            {'if': {'row_index': i}, 'backgroundColor': bg_color},
            {'if': {'row_index': i, 'column_id': 'price'}, 'color': text_color, 'fontWeight': '600'},
            {'if': {'row_index': i, 'column_id': 'quantity'}, 'color': 'white'},
        ]

    # Drop cumulative quantity before creating table
    df = df[['price', 'quantity']]
    
    # Format numbers with appropriate precision
    df['price'] = df['price'].apply(lambda x: f"{x:.2f}")
    df['quantity'] = df['quantity'].apply(lambda x: f"{x:.4f}")
    
    return dash_table.DataTable(
        columns=[
            {"name": "Price", "id": "price"},
            {"name": "Size", "id": "quantity"},
        ],
        data=df.to_dict('records'),
        style_as_list_view=True,
        style_cell={
            'backgroundColor': '#0c0c0c',
            'fontFamily': 'monospace',
            'fontSize': '13px',
            'border': 'none',
            'textAlign': 'right',
            'padding': '6px 8px',
            'letterSpacing': '0.4px',
        },
        style_header={
            'backgroundColor': '#0c0c0c',
            'color': '#666',
            'fontWeight': 'bold',
            'fontSize': '12px',
            'textTransform': 'uppercase',
            'letterSpacing': '1px',
            'borderBottom': '1px solid #222',
            'paddingBottom': '8px',
        },
        style_data_conditional=style_data_conditional,
        page_action='none',
        fixed_rows={'headers': True},
        style_table={'width': '100%', 'maxHeight': '300px', 'overflowY': 'auto'},
    )

def dropdown_option(title, options, default_value, _id):
    return html.Div(
        style={
            "marginLeft": "20px",
            "width": "180px"
        },
        children=[
            html.H4(
                title,
                style={
                    "color": "#9aa0a6",
                    "fontSize": "14px",
                    "fontWeight": "600",
                    "marginBottom": "8px",
                    "textAlign": "left"
                }
            ),
            dcc.Dropdown(
                options=options,
                value=default_value,
                id=_id,
                style={
                    "backgroundColor": "#0c0c0c",
                    "color": "#ffffff",
                },
                className="dropdown-dark"
            )
        ]
    )

# initial fetch for first render
initial_symbol = "SOLUSDT"
initial_agg_level = Decimal("0.1")
raw_asks, raw_bids, initial_mid_price = fetch_orderbook_data(symbol=initial_symbol, limit=100)
asks_df = aggregate_orderbook_levels(raw_asks, side='ask', agg_level=initial_agg_level).head(10)
bids_df = aggregate_orderbook_levels(raw_bids, side='bid', agg_level=initial_agg_level).head(10)

# LAYOUT (centered, card) 
app.layout = html.Div(
    style={
        "minHeight": "100vh",
        "display": "flex",
        "alignItems": "center",
        "justifyContent": "center",
        "backgroundColor": "#070707",
        "padding": "24px"
    },
    children=[
        html.Div(
            style={
                "width": "420px",                    # compact width
                "backgroundColor": "#0f0f10",
                "borderRadius": "12px",
                "boxShadow": "0 8px 30px rgba(0,0,0,0.6)",
                "padding": "18px",
                "textAlign": "center",
                "border": "1px solid rgba(255,255,255,0.04)"
            },
            children=[
                html.H4(
                    id='orderbook-title',
                    children="SOL/USDT Order Book",  # Initial value
                    style={
                        "marginBottom": "12px",
                        "fontWeight": "700",
                        "fontFamily": "Inter, sans-serif",
                        "color": "#ffffff",
                        "letterSpacing": "0.4px",
                        "fontSize": "20px"
                    }
                ),

                # ASKS (container with id so callback can update)
                html.Div(id='asks-table', children=[make_table(asks_df, 'red')]),

                # Middle price container
                html.Div([
                    html.Div(
                        "Middle Price",
                        style={
                            'color': '#9aa0a6',
                            'fontWeight': '600',
                            'fontSize': '12px',
                            'textTransform': 'uppercase',
                            'letterSpacing': '1px',
                            'marginBottom': '4px'
                        }
                    ),
                    html.Div(
                        id='mid-price-value',
                        children=f"${initial_mid_price:,.2f}",
                        style={
                            'color': '#ffffff',
                            'fontWeight': '700',
                            'fontSize': '18px',
                            'fontFamily': 'monospace',
                            'letterSpacing': '0.5px'
                        }
                    )
                ],
                style={
                    'textAlign': 'center',
                    'margin': '15px 0',
                    'padding': '10px',
                    'backgroundColor': '#0c0c0c',
                    'borderRadius': '8px',
                    'border': '1px solid rgba(255,255,255,0.04)'
                }),

                # BIDS
                html.Div(id='bids-table', children=[make_table(bids_df, 'green')]),

                # Interval for auto-refresh (2 seconds)
                dcc.Interval(id="auto-refresh", interval=2000, n_intervals=0),

                # optional manual refresh button (kept for convenience)
                dbc.Button(
                    "Refresh",
                    id="refresh",
                    color="secondary",
                    outline=True,
                    className="mt-3",
                    style={
                        "width": "100%",
                        "fontSize": "13px",
                        "borderRadius": "8px",
                        "borderColor": "#444",
                        "marginTop": "12px"
                    }
                ),
            ]
        ),

        # Dropdown options
        html.Div(
            style={
                "display": "flex",
                "justifyContent": "flex-start",
                "alignItems": "center",
                "marginTop": "20px",
                "backgroundColor": "#0f0f10",
                "padding": "15px",
                "borderRadius": "12px",
                "boxShadow": "0 4px 20px rgba(0,0,0,0.4)",
                "border": "1px solid rgba(255,255,255,0.04)"
            },
            children=[
                dropdown_option(
                    title="Aggregation Level",
                    options=[
                        {"label": "0.01", "value": "0.01"},
                        {"label": "0.1", "value": "0.1"},
                        {"label": "1.0", "value": "1"},
                        {"label": "10.0", "value": "10"},
                        {"label": "100.0", "value": "100"}
                    ],
                    default_value="0.1",
                    _id="aggregation_level"
                ),
                dropdown_option(
                    title="Trading Pair",
                    options=[
                        {"label": "SOL/USDT", "value": "SOLUSDT"},
                        {"label": "ETH/USDT", "value": "ETHUSDT"},
                        {"label": "UNI/USDT", "value": "UNIUSDT"},
                        {"label": "BTC/USDT", "value": "BTCUSDT"}
                    ],
                    default_value="SOLUSDT",
                    _id="symbol_value"
                )
            ]
        ),
    ]
)

# CALLBACK: auto-refresh every 2s 
@app.callback(
    Output('orderbook-title', 'children'),
    Output('mid-price-value', 'children'),
    Output('mid-price-value', 'style'),
    Output('asks-table', 'children'),
    Output('bids-table', 'children'),
    Input('aggregation_level', 'value'),
    Input('symbol_value', 'value'),
    Input("auto-refresh", "n_intervals"),
    Input("refresh", "n_clicks"),   # keep manual refresh as optional trigger as well
    prevent_initial_call=False  # Allow initial callback
)
def update_tables(agg_level, symbol, n_intervals, n_clicks):
    try:
        # Convert aggregation level from string to Decimal
        agg_level = Decimal(str(agg_level))
        
        # Format title (e.g., "BTC/USDT" from "BTCUSDT")
        base_quote = symbol.replace("USDT", "/USDT")
        title = f"{base_quote} Order Book"
        
        # Fetch fresh orderbook data with selected symbol
        raw_asks, raw_bids, mid_price = fetch_orderbook_data(symbol=symbol, limit=100)
        
        # Format mid price with dollar sign
        mid_price_display = f"${mid_price:,.2f}"
        
        # Style for mid price (default style with potential animation)
        mid_price_style = {
            'color': '#ffffff',
            'fontWeight': '700',
            'fontSize': '18px',
            'fontFamily': 'monospace',
            'letterSpacing': '0.5px',
            'transition': 'color 0.3s ease'
        }
        
        # Re-aggregate with selected aggregation level
        asks = aggregate_orderbook_levels(raw_asks, side='ask', agg_level=agg_level)
        bids = aggregate_orderbook_levels(raw_bids, side='bid', agg_level=agg_level)
        
        # Take top 10 levels for each
        asks = asks.head(10)
        bids = bids.head(10)
        
        # Create tables with updated data
        asks_table = make_table(asks, 'red')
        bids_table = make_table(bids, 'green')
        
        return title, mid_price_display, mid_price_style, asks_table, bids_table
        
    except Exception as e:
        print(f"Error updating tables: {e}")
        # Return empty/error state for all components
        empty_df = pd.DataFrame(columns=['price', 'quantity'])
        error_style = {
            'color': '#ff6b6b',
            'fontWeight': '700',
            'fontSize': '18px',
            'fontFamily': 'monospace',
            'letterSpacing': '0.5px',
        }
        return (
            "Error",  # title
            "N/A",   # mid price display
            error_style,  # mid price style
            make_table(empty_df, 'red'),  # asks table
            make_table(empty_df, 'green')  # bids table
        )

if __name__ == '__main__':
    app.run(debug=True)