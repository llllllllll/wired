def flat_repr(ob):
    """Normalize the whitespace of the repr of an object.

    Notes
    -----
    This makes nested tree reprs look much nicer when leaves contain ndarrays.
    """
    return ' '.join(repr(ob).split())
