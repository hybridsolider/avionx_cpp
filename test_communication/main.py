import sys

def hello(x,y):
    return int(x) + int(y)

def handle_request():
    operation = sys.argv[1]
    arg_list = sys.argv[2:]
    return eval(f"{operation}(*{arg_list})")

if __name__ == '__main__':
    sys.exit(handle_request())