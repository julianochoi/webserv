from datetime import datetime


def parse_imf_date(datestring: str) -> datetime:
    imf_date_format = "%a, %d %b %Y %H:%M:%S %Z"
    try:
        return datetime.strptime(datestring, imf_date_format)
    except:
        raise SyntaxError("Expected format: Sun, 06 Nov 1994 08:49:37 GMT")


def get_from_dict(key: str, d: dict):
    count = 0
    for k, v in d.items():
        if k.lower() == key:
            count += 1
            ret_val = v
    if count > 1:
        raise ValueError(f"Duplicated key: {key}")
    if count == 0:
        raise KeyError
    return ret_val
