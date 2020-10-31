import sys
import numpy as np
from numpy.random import randint, rand


def print_matrix(*args):
    for mat in args:
        if isinstance(mat, np.ndarray):
            print(mat.shape[0], mat.shape[1])
            for row in mat:
                print(*row)
        else:
            print(mat)


def random_matrix(n, m):
    return (rand(n, m) - 0.5) * 20.


def gen_tests():
    n, m = randint(1, 10, 2)
    mat1 = random_matrix(n, m)
    mat2 = random_matrix(n, m)
    mat3 = random_matrix(m, randint(1, 10))
    scalar = random_matrix(1, 1)[0][0]
    mat_sq = random_matrix(n, n)
    small = randint(1, 6)
    mat_sq_sm = random_matrix(small, small)

    print_matrix(mat1, mat2, mat1 + mat2)
    print_matrix(mat1 - mat2)
    print_matrix(mat3, mat1 @ mat3)
    print_matrix(scalar, scalar * mat1)
    print_matrix(-mat1)
    print_matrix(mat1.T)
    print_matrix(mat_sq, np.trace(mat_sq))
    print_matrix(mat_sq_sm, np.linalg.det(mat_sq_sm))


for _ in range(int(sys.argv[1])):
    gen_tests()
