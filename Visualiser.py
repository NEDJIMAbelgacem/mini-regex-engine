import sys
from graphviz import Digraph

g = Digraph('G', format="png", filename='tmp.gv')
g.attr(rankdir='LR', size='8,5')

f = open(sys.argv[1], "r")

alphabet_count, states_count, fstates_count, trans_table_size, start_state = f.readline().split()
alphabet_count, states_count, fstates_count, trans_table_size = map(int, [alphabet_count, states_count, fstates_count, trans_table_size])
alphabet = f.readline().split(' ')

g.attr('node', shape='square')
g.node(start_state)

states = f.readline().split()
fstates = f.readline().split()

g.attr('node', shape='circle')
for i in states:
    if i not in fstates:
        g.node(i)

g.attr('node', shape='doublecircle')

for i in fstates:
    g.node(i)

transitions = [f.readline().split() for i in range(trans_table_size)]

for s1, c, s2 in transitions:
    g.edge(s1, s2, c)

g.view()
