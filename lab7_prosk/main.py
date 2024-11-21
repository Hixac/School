import file_manip as man

l = man.read_file("el.txt")
t = man.count_el_below_arth(l)
man.write_result("output.txt", t)
