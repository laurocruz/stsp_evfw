#ifndef MYLEMONUTILS_DEFINE
#define MYLEMONUTILS_DEFINE
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>
#include <lemon/preflow.h>
#include <lemon/smart_graph.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/dijkstra.h>

#include <string>
#include <stdio.h>
using namespace lemon;
using namespace std;
typedef FullGraph Graph;
typedef Graph::Node Node;
typedef Graph::Edge Edge;
typedef Graph::EdgeIt EdgeIt;
typedef Graph::NodeIt NodeIt;
typedef Graph::IncEdgeIt IncEdgeIt;
typedef Graph::NodeMap<string> NodeName;
typedef Graph::NodeMap<double> NodePos;
typedef Graph::EdgeMap<double> EdgeWeight;
typedef Graph::EdgeMap<bool> EdgeSet;
typedef Graph::NodeMap<bool> CutMap;

typedef ListDigraph Digraph;
typedef Digraph::Node DiNode;
typedef Digraph::NodeIt DiNodeIt;
typedef Digraph::Arc Arc;
typedef Digraph::ArcIt ArcIt;
typedef Digraph::ArcMap<double> ArcWeight;
typedef Digraph::NodeMap<string> DiNodeName;
typedef Digraph::ArcMap<string> ArcName;
typedef Digraph::NodeMap<int> DiNodeColor;
typedef Digraph::ArcMap<int> ArcColor;
typedef Digraph::NodeMap<bool> DiCutMap;
typedef Digraph::NodeMap<double> DiNodePos;

#endif
