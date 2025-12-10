import networkx as nx
import matplotlib.pyplot as plt

def create_test_graph():
    G = nx.DiGraph()
    edges = [
        (0, 1, 2), (1, 2, 2), (2, 3, 3),
        (4, 5, 2), (5, 6, 2), (6, 7, 3),
        (8, 9, 2), (9,10, 2), (10,11,3),
        (12,13,2), (13,14,2), (14,15,3),

        (0, 4, 4), (1, 5, 4), (2, 6, 4), (3, 7, 4),
        (4, 8, 4), (5, 9, 4), (6,10, 4), (7,11,4),
        (8,12,4), (9,13,4), (10,14,4), (11,15,4),

        (1, 4, 3), (5, 2, 3),
        (6, 9, 3), (10, 7, 3),
        (12, 9, 5), (13,10, 5),
    ]
    for u, v, w in edges:
        G.add_edge(u, v, weight=w)
    return G

def draw_graph(G, title="Graph"):
    pos = nx.spring_layout(G, seed=1)
    edge_labels = nx.get_edge_attributes(G, 'weight')

    plt.figure(figsize=(6, 4))
    nx.draw(G, pos, with_labels=True, node_color='lightblue', node_size=800, arrows=True)
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
    plt.title(title)
    plt.show()

G = create_test_graph()

print("Original graph:")
draw_graph(G, "Original Graph")
