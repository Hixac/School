from typing import Tuple

def read_file(path: str) -> list[int]:
    f = open(path, 'r')
    l = list(map(int, f.readline().split()))
    return l

def count_el_below_arth(l: list[int]) -> Tuple[int, int]:
    average = sum(l) / len(l)
    return (sum(1 for i in range(len(l)) if l[i] < average), min(l))

def write_result(path: str, t: Tuple[int, int]) -> None:
    open(path, 'w').write(str(t[0]) + " " + str(t[1]))
