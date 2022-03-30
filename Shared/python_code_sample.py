import numpy as np
import random

def sample_test():
    #a = [1,2,3,4,5]
    #print("hige_1")
    #b = random.choice(a)
    #print("hoge_2")
    #return a[3]
    #x = np.array([1.1,2.2,3,4,5], dtype=np.float)
    #x = np.array([1,2,3,4,5], dtype=np.int)
    #xx = []
    #for i in range(len(x)):
    #    xx.append(x[i])
    #print(xx)
    xx = [[1,2,3,4,5], [2,2,3,4,5]]
    print(xx)
    return xx

def sample_test2(*args):
    for i in range(len(args)):
        print(args[i])
    return [111, 222, 333]

def sample_test3(*args):
    for i in range(len(args)):
        print(args[i])
    return ["re", "tu", "rn"]

def sample_test_actual(*args):
    for i in range(len(args)):
        print(args[i])
    return [0.1, 0.2, 0.3]

def sample_test_numpy1(*args):
    x = np.array([[1, 2, 3], [4, 5, 6]])
    for i in range(len(args)):
        print(args[i])
    mat = args[0]
    length = len(mat)
    dim = len(mat[0])
    x = np.zeros((length, dim))
    for i in range(length):
        for j in range(dim):
            x[i, j] = mat[i][j]
    #print(x)
    #x = x.tolist()
    #x = np.array([1.1, 3.3, 4.4])
    return x


def sample_test_numpy2(dic):
    for key in dic:
        print(key)
        data = dic[key]
        if key == "data":
            for data_key in data:
                print(key, data[data_key].shape)
        elif key == "params_i":
            print("int", data)
        elif key == "params_d":
            print("double", data)
        elif key == "params_s":
            print("str", data)
    x = np.array([[1.1, 2, 3], [4.4, 5, 6]])

    return x


#if __name__ == "__main__":
#    #print(sample_test())
#    print(sample_test_numpy())