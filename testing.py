from sklearn import tree #1
from sklearn.datasets import load_iris #2
import numpy as np

def main():
    #test1()
    test2(part=2)

def test1():
    #1
    features = [[140, 1], [130, 1], [150, 0], [170, 0]]
    labels = [0, 0, 1, 1]
    clf = tree.DecisionTreeClassifier()
    clf = clf.fit(features, labels)
    print(clf.predict([[160, 1]]))

def test2(part=1):
    iris = load_iris()
    print(iris)

    if part == 1:
        # Data set info:
        print(iris.feature_names)
        print(iris.target_names)
        print(iris.data[0])
        print(iris.target[0])
        for i, label in enumerate(iris.target):
            print("Example {0}: label {1}, features {2}".format(i, label, iris.data[i]))

    else:
        # Training data
        test_index = [0, 50, 100]
        train_target = np.delete(iris.target, test_index)
        train_data = np.delete(iris.data, test_index, axis=0)

        # Testing data
        test_target = iris.target[test_index]
        test_data = iris.data[test_index]

        clf = tree.DecisionTreeClassifier()
        clf.fit(train_data, train_target)

        print(test_target)
        print(clf.predict(test_data))

if __name__ == '__main__':
    main()