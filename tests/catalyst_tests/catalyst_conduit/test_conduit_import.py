try:
    import catalyst_conduit
    import catalyst_conduit.blueprint
    import catalyst_conduit.blueprint.mesh
    import catalyst_conduit.blueprint.table
    import catalyst_conduit.blueprint.mcarray
except ImportError as error:
    print("ERROR : Could not import conduit modules")
    print(error)
    exit(1)

print(catalyst_conduit.about())
print(catalyst_conduit.blueprint.about())
