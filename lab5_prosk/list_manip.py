
def sum_pos(l: list, until: int) -> int:
    return sum(l[i] for i in range(until) if l[i] > 0) if until < len(l) else 0

def sum_neg(l: list, fr: int) -> int:
    return sum(l[i] for i in range(fr, len(l)) if l[i] < 0) if fr < len(l) else 0
