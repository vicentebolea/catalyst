try:
    import catalyst
except ImportError as ex:
    print("ERROR : Could not import catalyst module")
    print(ex)
    exit(1)


def check_if_method_exists(key):
    try:
        return hasattr(catalyst, key) and callable(getattr(catalyst, key))
    except AttributeError:
        print(f"ERROR: Method does not exist: {key}")
    return False


assert check_if_method_exists("initialize")
assert check_if_method_exists("execute")
assert check_if_method_exists("finalize")
assert check_if_method_exists("results")
assert check_if_method_exists("about")
