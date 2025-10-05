import requests
import os
import json
from dotenv import load_dotenv

load_dotenv()

BASE_URL = "https://www.alphavantage.co/query"
API_KEY = os.getenv("ALPHA_VANTAGE_API_KEY") 

def get_intraday_data(symbol: str, interval: str = "1min", outputsize: str = "compact", save_file: bool = True):
    """
    Fetch TIME_SERIES_INTRADAY and return the latest timestamp + candle.
    interval: '1min', '5min', '15min', '30min', '60min'
    outputsize: 'compact' (latest) or 'full' (full history)
    """
    params = {
        "function": "TIME_SERIES_INTRADAY",
        "symbol": symbol,
        "interval": interval,
        "outputsize": outputsize,
        "apikey": API_KEY, 
        "datatype": "json"
    }

    response = requests.get(BASE_URL, params=params, timeout=10)
    response.raise_for_status()  # raise HTTPError for non-200
    data = response.json()

    if not data:
        raise RuntimeError("Empty response from Alpha Vantage.")
    if "Note" in data:
        raise RuntimeError("Alpha Vantage Note: " + data["Note"])   # Usually rate limit message
    if "Error Message" in data:
        raise RuntimeError("Alpha Vantage Error: " + data["Error Message"])

    pretty = json.dumps(data, indent=4)
    print(pretty)

    return data

if __name__ == "__main__":
    symbol = "AAPL"   # change to desired symbol
    interval = "5min"  # change to desired interval

    try:
        data = get_intraday_data(symbol, interval, outputsize="compact", save_file=True)
    except Exception as e:
        print("Error fetching intraday data:", e)