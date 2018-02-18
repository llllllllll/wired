from functools import partial

import numpy as np


array = np.array
dot = np.dot
expr = np.exp
ndim = np.ndim
product = partial(np.product, axis=0)
shape = np.shape
sum = partial(np.sum, axis=0)


def expr():
    # BEGIN GENERATED CODE
    # PSYCHE-GENERATED-CODE
    # END GENERATED CODE

    return result


if __name__ == '__main__':
    print(expr())
