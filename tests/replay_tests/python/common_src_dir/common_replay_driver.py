import sys

import catalyst
import catalyst_conduit as conduit

import numpy


CATALYST_USE_MPI = False
if "--use-mpi" in sys.argv:
    CATALYST_USE_MPI = True

if CATALYST_USE_MPI:
    from mpi4py import MPI


def main(path):
    comm = None
    if CATALYST_USE_MPI:
        comm = MPI.COMM_WORLD

    node = conduit.Node()

    # the catalyst implementation expect specific bitwidths
    # make sure we honour them see also:
    # https://llnl-conduit.readthedocs.io/en/v0.8.8/tutorial_python_basics.html#bitwidth-style-types

    if CATALYST_USE_MPI:
        node["mpi_comm"] = numpy.int64(comm.py2f())

    a = 10
    node["data"] = numpy.int32(a)
    node["stage"] = "initialize"
    node["catalyst_load"]["implementation"] = "replay"
    node["catalyst_load"]["search_paths"]["example"] = path

    catalyst.initialize(node)

    node["stage"] = "execute"
    a += 1
    node["data"] = numpy.int32(a)
    for _ in range(3):
        catalyst.execute(node)

    node["stage"] = "finalize"
    a += 1
    node["data"] = numpy.int32(a)

    catalyst.finalize(node)

    node["stage"] = "about"
    catalyst.about(node)


if __name__ == "__main__":
    try:
        if len(sys.argv) < 2:
            print("ERROR: wrong argument count")
            exit(1)
        main(sys.argv[1])
        exit(0)
    except catalyst.CatalystError as ex:
        print("ERROR: Internal catalyst error ", str(ex))
    except RuntimeError as ex:
        print("ERROR: Validation error returned value of catalyst API", str(ex))

    exit(1)
