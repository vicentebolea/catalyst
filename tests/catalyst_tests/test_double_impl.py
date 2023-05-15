import catalyst
import catalyst_conduit as conduit


def test_double():

    params = conduit.Node()
    params["data"] = 1.0
    params["catalyst_load"]["implementation"] = "double"
    print(params)
    catalyst.initialize(params)
    params = None

    about = conduit.Node()
    catalyst.about(about)
    if about["data"] != 1.0:
        raise RuntimeError("Failed to get `data` from `about`")
    about = None

    exe = conduit.Node()
    exe["data"] = 2.0
    catalyst.execute(exe)
    exe = None

    result = conduit.Node()
    catalyst.results(result)

    if result["data"] != 2.0:
        raise RuntimeError("failed to get `data` from `result`")
    result = None

    final = conduit.Node()
    catalyst.finalize(final)

    if final["data"] != 2.0:
        raise RuntimeError("failed to get `data` from `finalize`")
    final = None


if __name__ == "__main__":
    try:
        test_double()
        exit(0)
    except catalyst.CatalystError as ex:
        print("ERROR: Internal catalyst error ", str(ex))
    except RuntimeError as ex:
        print("ERROR: Validation error returned value of catalyst API", str(ex))

    exit(1)
