from dash import Dash, dcc, html, Input, Output, dash_table
import dash_bootstrap_components as dbc
import plotly.graph_objects as go
import pandas as pd
import numpy as np
import math
import requests
from decimal import Decimal

app = Dash(__name__, external_stylesheets=[dbc.themes.CYBORG])

def fetch_orderbook_data(symbol: str, limit: int):
    url = "https://api.binance.com/api/v3/depth"
    params = {
        "symbol": symbol,
        "limit": limit
    }

    response = requests.get(url, params=params)
    response.raise_for_status()
    data = response.json()

    asks = pd.DataFrame(data['asks'], columns=['price', 'quantity'], dtype=float).sort_values(by='price', ascending=False)
    bids = pd.DataFrame(data['bids'], columns=['price', 'quantity'], dtype=float).sort_values(by='price', ascending=False)

    return asks, bids

# Dash DataTable properties
def make_table(df: pd.DataFrame, color: str):
    """
    Create styled DataTable for asks (red) or bids (green)
    """
    df = df.round(3)
    max_qty = df['quantity'].max() if not df.empty else 1

    style_data_conditional = []
    for i, row in df.iterrows():
        pct = row['quantity'] / max_qty
        alpha = 0.1 + 0.4 * pct  # opacity by depth
        bg_color = f'rgba(255,0,0,{alpha})' if color == 'red' else f'rgba(0,255,0,{alpha})'
        text_color = '#ff6b6b' if color == 'red' else '#00ff99'
        style_data_conditional += [
            {'if': {'row_index': i}, 'backgroundColor': bg_color},
            {'if': {'row_index': i, 'column_id': 'price'}, 'color': text_color, 'fontWeight': '600'},
            {'if': {'row_index': i, 'column_id': 'quantity'}, 'color': 'white'},
        ]

    return dash_table.DataTable(
        columns=[
            {"name": "price", "id": "price", "type": "numeric"},
            {"name": "quantity", "id": "quantity", "type": "numeric"},
        ],
        data=df.to_dict('records'),
        style_as_list_view=True,
        style_cell={
            'backgroundColor': '#0c0c0c',
            'fontFamily': 'monospace',
            'fontSize': '14px',
            'border': 'none',
            'textAlign': 'right',
            'padding': '5px',
        },
        style_header={'display': 'none'},
        style_data_conditional=style_data_conditional,
        page_action='none',
        fixed_rows={'headers': False},
        style_table={'width': '100%', 'maxHeight': '220px', 'overflowY': 'auto'},
    )

# initial fetch for first render
asks_df, bids_df = fetch_orderbook_data(symbol="SOLUSDT", limit=10)

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
                    "SOL/USDT Order Book",
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

                html.Div(
                    "Middle Price",
                    style={
                        'textAlign': 'center',
                        'color': '#9aa0a6',
                        'margin': '10px 0',
                        'fontWeight': '700',
                        'fontSize': '14px'
                    }
                ),

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
        )
    ]
)

# CALLBACK: auto-refresh every 2s 
@app.callback(
    Output('asks-table', 'children'),
    Output('bids-table', 'children'),
    Input("auto-refresh", "n_intervals"),
    Input("refresh", "n_clicks"),   # keep manual refresh as optional trigger as well
)
def update_tables(n_intervals, n_clicks):
    asks, bids = fetch_orderbook_data(symbol="SOLUSDT", limit=10)   # fetch returns asks, bids
    asks_table = make_table(asks, 'red')
    bids_table = make_table(bids, 'green')
    return asks_table, bids_table

if __name__ == '__main__':
    app.run(debug=True)