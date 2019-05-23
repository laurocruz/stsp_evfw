/* -*- mode: C++; indent-tabs-mode: nil; -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library.
 *
 * Copyright (C) 2003-2013
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#ifndef LEMON_DIJKSTRA_STEINER_H
#define LEMON_DIJKSTRA_STEINER_H

///\ingroup shortest_path
///\file
///\brief Dijkstra algorithm.

#include <limits>
#include <lemon/list_graph.h>
#include <lemon/bin_heap.h>
#include <lemon/bits/path_dump.h>
#include <lemon/core.h>
#include <lemon/error.h>
#include <lemon/maps.h>
#include <lemon/path.h>

namespace lemon {

  /// \brief Default operation traits for the Dijkstra algorithm class.
  ///
  /// This operation traits class defines all computational operations and
  /// constants which are used in the Dijkstra algorithm.
  template <typename V>
  struct Dijkstra_SteinerDefaultOperationTraits {
    /// \e
    typedef V Value;
    /// \brief Gives back the zero value of the type.
    static Value zero() {
      return static_cast<Value>(0);
    }
    /// \brief Gives back the sum of the given two elements.
    static Value plus(const Value& left, const Value& right) {
      return left + right;
    }
    /// \brief Gives back true only if the first value is less than the second.
    static bool less(const Value& left, const Value& right) {
      return left < right;
    }
  };

  ///Default traits class of Dijkstra class.

  ///Default traits class of Dijkstra class.
  ///\tparam GR The type of the digraph.
  ///\tparam LEN The type of the length map.
  template<typename GR, typename LEN>
  struct Dijkstra_SteinerDefaultTraits
  {
    ///The type of the digraph the algorithm runs on.
    typedef GR Digraph;

    ///The type of the map that stores the arc lengths.

    ///The type of the map that stores the arc lengths.
    ///It must conform to the \ref concepts::ReadMap "ReadMap" concept.
    typedef LEN LengthMap;
    ///The type of the arc lengths.
    typedef typename LEN::Value Value;

    /// Operation traits for %Dijkstra algorithm.

    /// This class defines the operations that are used in the algorithm.
    /// \see DijkstraDefaultOperationTraits
    typedef Dijkstra_SteinerDefaultOperationTraits<Value> OperationTraits;

    /// The cross reference type used by the heap.

    /// The cross reference type used by the heap.
    /// Usually it is \c Digraph::NodeMap<int>.
    typedef typename Digraph::template NodeMap<int> HeapCrossRef;
    ///Instantiates a \c HeapCrossRef.

    ///This function instantiates a \ref HeapCrossRef.
    /// \param g is the digraph, to which we would like to define the
    /// \ref HeapCrossRef.
    static HeapCrossRef *createHeapCrossRef(const Digraph &g)
    {
      return new HeapCrossRef(g);
    }

    ///The heap type used by the %Dijkstra algorithm.

    ///The heap type used by the Dijkstra algorithm.
    ///
    ///\sa BinHeap
    ///\sa Dijkstra
    typedef BinHeap<typename LEN::Value, HeapCrossRef, std::less<Value> > Heap;
    ///Instantiates a \c Heap.

    ///This function instantiates a \ref Heap.
    static Heap *createHeap(HeapCrossRef& r)
    {
      return new Heap(r);
    }

    ///\brief The type of the map that stores the predecessor
    ///arcs of the shortest paths.
    ///
    ///The type of the map that stores the predecessor
    ///arcs of the shortest paths.
    ///It must conform to the \ref concepts::WriteMap "WriteMap" concept.
    typedef typename Digraph::template NodeMap<typename Digraph::Arc> PredMap;
    ///Instantiates a \c PredMap.

    ///This function instantiates a \ref PredMap.
    ///\param g is the digraph, to which we would like to define the
    ///\ref PredMap.
    static PredMap *createPredMap(const Digraph &g)
    {
      return new PredMap(g);
    }

    ///The type of the map that indicates which nodes are processed.

    ///The type of the map that indicates which nodes are processed.
    ///It must conform to the \ref concepts::WriteMap "WriteMap" concept.
    ///By default, it is a NullMap.
    typedef NullMap<typename Digraph::Node,bool> ProcessedMap;
    ///Instantiates a \c ProcessedMap.

    ///This function instantiates a \ref ProcessedMap.
    ///\param g is the digraph, to which
    ///we would like to define the \ref ProcessedMap.
#ifdef DOXYGEN
    static ProcessedMap *createProcessedMap(const Digraph &g)
#else
    static ProcessedMap *createProcessedMap(const Digraph &)
#endif
    {
      return new ProcessedMap();
    }

    ///The type of the map that stores the distances of the nodes.

    ///The type of the map that stores the distances of the nodes.
    ///It must conform to the \ref concepts::WriteMap "WriteMap" concept.
    typedef typename Digraph::template NodeMap<typename LEN::Value> DistMap;
    ///Instantiates a \c DistMap.

    ///This function instantiates a \ref DistMap.
    ///\param g is the digraph, to which we would like to define
    ///the \ref DistMap.
    static DistMap *createDistMap(const Digraph &g)
    {
      return new DistMap(g);
    }
  };

  ///%Dijkstra algorithm class.

  /// \ingroup shortest_path
  ///This class provides an efficient implementation of the %Dijkstra algorithm.
  ///
  ///The %Dijkstra algorithm solves the single-source shortest path problem
  ///when all arc lengths are non-negative. If there are negative lengths,
  ///the BellmanFord algorithm should be used instead.
  ///
  ///The arc lengths are passed to the algorithm using a
  ///\ref concepts::ReadMap "ReadMap",
  ///so it is easy to change it to any kind of length.
  ///The type of the length is determined by the
  ///\ref concepts::ReadMap::Value "Value" of the length map.
  ///It is also possible to change the underlying priority heap.
  ///
  ///There is also a \ref dijkstra() "function-type interface" for the
  ///%Dijkstra algorithm, which is convenient in the simplier cases and
  ///it can be used easier.
  ///
  ///\tparam GR The type of the digraph the algorithm runs on.
  ///The default type is \ref ListDigraph.
  ///\tparam LEN A \ref concepts::ReadMap "readable" arc map that specifies
  ///the lengths of the arcs.
  ///It is read once for each arc, so the map may involve in
  ///relatively time consuming process to compute the arc lengths if
  ///it is necessary. The default map type is \ref
  ///concepts::Digraph::ArcMap "GR::ArcMap<int>".
  ///\tparam TR The traits class that defines various types used by the
  ///algorithm. By default, it is \ref DijkstraDefaultTraits
  ///"DijkstraDefaultTraits<GR, LEN>".
  ///In most cases, this parameter should not be set directly,
  ///consider to use the named template parameters instead.
#ifdef DOXYGEN
  template <typename GR, typename LEN, typename TR>
#else
  template <typename GR=ListDigraph,
            typename LEN=typename GR::template ArcMap<int>,
            typename TR=Dijkstra_SteinerDefaultTraits<GR,LEN> >
#endif
  class Dijkstra_Steiner {
  public:

    ///The type of the digraph the algorithm runs on.
    typedef typename TR::Digraph Digraph;

    ///The type of the arc lengths.
    typedef typename TR::Value Value;
    ///The type of the map that stores the arc lengths.
    typedef typename TR::LengthMap LengthMap;
    ///\brief The type of the map that stores the predecessor arcs of the
    ///shortest paths.
    typedef typename TR::PredMap PredMap;
    ///The type of the map that stores the distances of the nodes.
    typedef typename TR::DistMap DistMap;
    ///The type of the map that indicates which nodes are processed.
    typedef typename TR::ProcessedMap ProcessedMap;
    ///The type of the paths.
    typedef PredMapPath<Digraph, PredMap> Path;
    ///The cross reference type used for the current heap.
    typedef typename TR::HeapCrossRef HeapCrossRef;
    ///The heap type used by the algorithm.
    typedef typename TR::Heap Heap;
    /// \brief The \ref lemon::DijkstraDefaultOperationTraits
    /// "operation traits class" of the algorithm.
    typedef typename TR::OperationTraits OperationTraits;

    ///The \ref lemon::DijkstraDefaultTraits "traits class" of the algorithm.
    typedef TR Traits;

  private:

    typedef typename Digraph::Node Node;
    typedef typename Digraph::NodeIt NodeIt;
    typedef typename Digraph::Arc Arc;
    typedef typename Digraph::OutArcIt OutArcIt;

    //Pointer to the underlying digraph.
    const Digraph *G;
    //Pointer to the length map.
    const LengthMap *_length;
    //Pointer to the map of predecessors arcs.
    PredMap *_pred;
    //Indicates if _pred is locally allocated (true) or not.
    bool local_pred;
    //Pointer to the map of distances.
    DistMap *_dist;
    //Indicates if _dist is locally allocated (true) or not.
    bool local_dist;
    //Pointer to the map of processed status of the nodes.
    ProcessedMap *_processed;
    //Indicates if _processed is locally allocated (true) or not.
    bool local_processed;
    //Pointer to the heap cross references.
    HeapCrossRef *_heap_cross_ref;
    //Indicates if _heap_cross_ref is locally allocated (true) or not.
    bool local_heap_cross_ref;
    //Pointer to the heap.
    Heap *_heap;
    //Indicates if _heap is locally allocated (true) or not.
    bool local_heap;

    //Creates the maps if necessary.
    void create_maps()
    {
      if(!_pred) {
        local_pred = true;
        _pred = Traits::createPredMap(*G);
      }
      if(!_dist) {
        local_dist = true;
        _dist = Traits::createDistMap(*G);
      }
      if(!_processed) {
        local_processed = true;
        _processed = Traits::createProcessedMap(*G);
      }
      if (!_heap_cross_ref) {
        local_heap_cross_ref = true;
        _heap_cross_ref = Traits::createHeapCrossRef(*G);
      }
      if (!_heap) {
        local_heap = true;
        _heap = Traits::createHeap(*_heap_cross_ref);
      }
    }

  public:

    typedef Dijkstra_Steiner Create;

    ///\name Named Template Parameters

    ///@{

    template <class T>
    struct SetPredMapTraits : public Traits {
      typedef T PredMap;
      static PredMap *createPredMap(const Digraph &)
      {
        LEMON_ASSERT(false, "PredMap is not initialized");
        return 0; // ignore warnings
      }
    };
    ///\brief \ref named-templ-param "Named parameter" for setting
    ///\c PredMap type.
    ///
    ///\ref named-templ-param "Named parameter" for setting
    ///\c PredMap type.
    ///It must conform to the \ref concepts::WriteMap "WriteMap" concept.
    template <class T>
    struct SetPredMap
      : public Dijkstra_Steiner< Digraph, LengthMap, SetPredMapTraits<T> > {
      typedef Dijkstra_Steiner< Digraph, LengthMap, SetPredMapTraits<T> > Create;
    };

    template <class T>
    struct SetDistMapTraits : public Traits {
      typedef T DistMap;
      static DistMap *createDistMap(const Digraph &)
      {
        LEMON_ASSERT(false, "DistMap is not initialized");
        return 0; // ignore warnings
      }
    };
    ///\brief \ref named-templ-param "Named parameter" for setting
    ///\c DistMap type.
    ///
    ///\ref named-templ-param "Named parameter" for setting
    ///\c DistMap type.
    ///It must conform to the \ref concepts::WriteMap "WriteMap" concept.
    template <class T>
    struct SetDistMap
      : public Dijkstra_Steiner< Digraph, LengthMap, SetDistMapTraits<T> > {
      typedef Dijkstra_Steiner< Digraph, LengthMap, SetDistMapTraits<T> > Create;
    };

    template <class T>
    struct SetProcessedMapTraits : public Traits {
      typedef T ProcessedMap;
      static ProcessedMap *createProcessedMap(const Digraph &)
      {
        LEMON_ASSERT(false, "ProcessedMap is not initialized");
        return 0; // ignore warnings
      }
    };
    ///\brief \ref named-templ-param "Named parameter" for setting
    ///\c ProcessedMap type.
    ///
    ///\ref named-templ-param "Named parameter" for setting
    ///\c ProcessedMap type.
    ///It must conform to the \ref concepts::WriteMap "WriteMap" concept.
    template <class T>
    struct SetProcessedMap
      : public Dijkstra_Steiner< Digraph, LengthMap, SetProcessedMapTraits<T> > {
      typedef Dijkstra_Steiner< Digraph, LengthMap, SetProcessedMapTraits<T> > Create;
    };

    struct SetStandardProcessedMapTraits : public Traits {
      typedef typename Digraph::template NodeMap<bool> ProcessedMap;
      static ProcessedMap *createProcessedMap(const Digraph &g)
      {
        return new ProcessedMap(g);
      }
    };
    ///\brief \ref named-templ-param "Named parameter" for setting
    ///\c ProcessedMap type to be <tt>Digraph::NodeMap<bool></tt>.
    ///
    ///\ref named-templ-param "Named parameter" for setting
    ///\c ProcessedMap type to be <tt>Digraph::NodeMap<bool></tt>.
    ///If you don't set it explicitly, it will be automatically allocated.
    struct SetStandardProcessedMap
      : public Dijkstra_Steiner< Digraph, LengthMap, SetStandardProcessedMapTraits > {
      typedef Dijkstra_Steiner< Digraph, LengthMap, SetStandardProcessedMapTraits >
      Create;
    };

    template <class H, class CR>
    struct SetHeapTraits : public Traits {
      typedef CR HeapCrossRef;
      typedef H Heap;
      static HeapCrossRef *createHeapCrossRef(const Digraph &) {
        LEMON_ASSERT(false, "HeapCrossRef is not initialized");
        return 0; // ignore warnings
      }
      static Heap *createHeap(HeapCrossRef &)
      {
        LEMON_ASSERT(false, "Heap is not initialized");
        return 0; // ignore warnings
      }
    };
    ///\brief \ref named-templ-param "Named parameter" for setting
    ///heap and cross reference types
    ///
    ///\ref named-templ-param "Named parameter" for setting heap and cross
    ///reference types. If this named parameter is used, then external
    ///heap and cross reference objects must be passed to the algorithm
    ///using the \ref heap() function before calling \ref run(Node) "run()"
    ///or \ref init().
    ///\sa SetStandardHeap
    template <class H, class CR = typename Digraph::template NodeMap<int> >
    struct SetHeap
      : public Dijkstra_Steiner< Digraph, LengthMap, SetHeapTraits<H, CR> > {
      typedef Dijkstra_Steiner< Digraph, LengthMap, SetHeapTraits<H, CR> > Create;
    };

    template <class H, class CR>
    struct SetStandardHeapTraits : public Traits {
      typedef CR HeapCrossRef;
      typedef H Heap;
      static HeapCrossRef *createHeapCrossRef(const Digraph &G) {
        return new HeapCrossRef(G);
      }
      static Heap *createHeap(HeapCrossRef &R)
      {
        return new Heap(R);
      }
    };
    ///\brief \ref named-templ-param "Named parameter" for setting
    ///heap and cross reference types with automatic allocation
    ///
    ///\ref named-templ-param "Named parameter" for setting heap and cross
    ///reference types with automatic allocation.
    ///They should have standard constructor interfaces to be able to
    ///automatically created by the algorithm (i.e. the digraph should be
    ///passed to the constructor of the cross reference and the cross
    ///reference should be passed to the constructor of the heap).
    ///However, external heap and cross reference objects could also be
    ///passed to the algorithm using the \ref heap() function before
    ///calling \ref run(Node) "run()" or \ref init().
    ///\sa SetHeap
    template <class H, class CR = typename Digraph::template NodeMap<int> >
    struct SetStandardHeap
      : public Dijkstra_Steiner< Digraph, LengthMap, SetStandardHeapTraits<H, CR> > {
      typedef Dijkstra_Steiner< Digraph, LengthMap, SetStandardHeapTraits<H, CR> >
      Create;
    };

    template <class T>
    struct SetOperationTraitsTraits : public Traits {
      typedef T OperationTraits;
    };

    /// \brief \ref named-templ-param "Named parameter" for setting
    ///\c OperationTraits type
    ///
    ///\ref named-templ-param "Named parameter" for setting
    ///\c OperationTraits type.
    /// For more information, see \ref DijkstraDefaultOperationTraits.
    template <class T>
    struct SetOperationTraits
      : public Dijkstra_Steiner<Digraph, LengthMap, SetOperationTraitsTraits<T> > {
      typedef Dijkstra_Steiner<Digraph, LengthMap, SetOperationTraitsTraits<T> >
      Create;
    };

    ///@}

  protected:

    Dijkstra_Steiner() {}

  public:

    ///Constructor.

    ///Constructor.
    ///\param g The digraph the algorithm runs on.
    ///\param length The length map used by the algorithm.
    Dijkstra_Steiner(const Digraph& g, const LengthMap& length) :
      G(&g), _length(&length),
      _pred(NULL), local_pred(false),
      _dist(NULL), local_dist(false),
      _processed(NULL), local_processed(false),
      _heap_cross_ref(NULL), local_heap_cross_ref(false),
      _heap(NULL), local_heap(false)
    { }

    ///Destructor.
    ~Dijkstra_Steiner()
    {
      if(local_pred) delete _pred;
      if(local_dist) delete _dist;
      if(local_processed) delete _processed;
      if(local_heap_cross_ref) delete _heap_cross_ref;
      if(local_heap) delete _heap;
    }

    ///Sets the length map.

    ///Sets the length map.
    ///\return <tt> (*this) </tt>
    Dijkstra_Steiner &lengthMap(const LengthMap &m)
    {
      _length = &m;
      return *this;
    }

    ///Sets the map that stores the predecessor arcs.

    ///Sets the map that stores the predecessor arcs.
    ///If you don't use this function before calling \ref run(Node) "run()"
    ///or \ref init(), an instance will be allocated automatically.
    ///The destructor deallocates this automatically allocated map,
    ///of course.
    ///\return <tt> (*this) </tt>
    Dijkstra_Steiner &predMap(PredMap &m)
    {
      if(local_pred) {
        delete _pred;
        local_pred=false;
      }
      _pred = &m;
      return *this;
    }

    ///Sets the map that indicates which nodes are processed.

    ///Sets the map that indicates which nodes are processed.
    ///If you don't use this function before calling \ref run(Node) "run()"
    ///or \ref init(), an instance will be allocated automatically.
    ///The destructor deallocates this automatically allocated map,
    ///of course.
    ///\return <tt> (*this) </tt>
    Dijkstra_Steiner &processedMap(ProcessedMap &m)
    {
      if(local_processed) {
        delete _processed;
        local_processed=false;
      }
      _processed = &m;
      return *this;
    }

    ///Sets the map that stores the distances of the nodes.

    ///Sets the map that stores the distances of the nodes calculated by the
    ///algorithm.
    ///If you don't use this function before calling \ref run(Node) "run()"
    ///or \ref init(), an instance will be allocated automatically.
    ///The destructor deallocates this automatically allocated map,
    ///of course.
    ///\return <tt> (*this) </tt>
    Dijkstra_Steiner &distMap(DistMap &m)
    {
      if(local_dist) {
        delete _dist;
        local_dist=false;
      }
      _dist = &m;
      return *this;
    }

    ///Sets the heap and the cross reference used by algorithm.

    ///Sets the heap and the cross reference used by algorithm.
    ///If you don't use this function before calling \ref run(Node) "run()"
    ///or \ref init(), heap and cross reference instances will be
    ///allocated automatically.
    ///The destructor deallocates these automatically allocated objects,
    ///of course.
    ///\return <tt> (*this) </tt>
    Dijkstra_Steiner &heap(Heap& hp, HeapCrossRef &cr)
    {
      if(local_heap_cross_ref) {
        delete _heap_cross_ref;
        local_heap_cross_ref=false;
      }
      _heap_cross_ref = &cr;
      if(local_heap) {
        delete _heap;
        local_heap=false;
      }
      _heap = &hp;
      return *this;
    }

  private:

    void finalizeNodeData(Node v,Value dst)
    {
      _processed->set(v,true);
      _dist->set(v, dst);
    }

  public:

    ///\name Execution Control
    ///The simplest way to execute the %Dijkstra algorithm is to use
    ///one of the member functions called \ref run(Node) "run()".\n
    ///If you need better control on the execution, you have to call
    ///\ref init() first, then you can add several source nodes with
    ///\ref addSource(). Finally the actual path computation can be
    ///performed with one of the \ref start() functions.

    ///@{

    ///\brief Initializes the internal data structures.
    ///
    ///Initializes the internal data structures.
    void init()
    {
      create_maps();
      _heap->clear();
      for ( NodeIt u(*G) ; u!=INVALID ; ++u ) {
        _pred->set(u,INVALID);
        _processed->set(u,false);
        _heap_cross_ref->set(u,Heap::PRE_HEAP);
      }
    }

    ///Adds a new source node.

    ///Adds a new source node to the priority heap.
    ///The optional second parameter is the initial distance of the node.
    ///
    ///The function checks if the node has already been added to the heap and
    ///it is pushed to the heap only if either it was not in the heap
    ///or the shortest path found till then is shorter than \c dst.
    void addSource(Node s,Value dst=OperationTraits::zero())
    {
      if(_heap->state(s) != Heap::IN_HEAP) {
        _heap->push(s,dst);
      } else if(OperationTraits::less((*_heap)[s], dst)) {
        _heap->set(s,dst);
        _pred->set(s,INVALID);
      }
    }

    ///Processes the next node in the priority heap

    ///Processes the next node in the priority heap.
    ///
    ///\return The processed node.
    ///
    ///\warning The priority heap must not be empty.
    Node processNextNode()
    {
      Node v=_heap->top();
      Value oldvalue=_heap->prio();
      _heap->pop();
      finalizeNodeData(v,oldvalue);

      for(OutArcIt e(*G,v); e!=INVALID; ++e) {
        Node w=G->target(e);
        switch(_heap->state(w)) {
        case Heap::PRE_HEAP:
          _heap->push(w,OperationTraits::plus(oldvalue, (*_length)[e]));
          _pred->set(w,e);
          break;
        case Heap::IN_HEAP:
          {
            Value newvalue = OperationTraits::plus(oldvalue, (*_length)[e]);
            if ( OperationTraits::less(newvalue, (*_heap)[w]) ) {
              _heap->decrease(w, newvalue);
              _pred->set(w,e);
            }
          }
          break;
        case Heap::POST_HEAP:
          break;
        }
      }
      return v;
    }

    Node processNextNode_Steiner(Graph::NodeMap<bool> * terminals, int * number_reached_terminals)
    {
      Node v=_heap->top();

      if((*terminals)[v] == true){
    	  (*number_reached_terminals)++;
      }

      Value oldvalue=_heap->prio();
      _heap->pop();
      finalizeNodeData(v,oldvalue);

      for(OutArcIt e(*G,v); e!=INVALID; ++e) {
        Node w=G->target(e);
        switch(_heap->state(w)) {
        case Heap::PRE_HEAP:
          _heap->push(w,OperationTraits::plus(oldvalue, (*_length)[e]));
          _pred->set(w,e);
          break;
        case Heap::IN_HEAP:
          {
            Value newvalue = OperationTraits::plus(oldvalue, (*_length)[e]);
            if ( OperationTraits::less(newvalue, (*_heap)[w]) ) {
              _heap->decrease(w, newvalue);
              _pred->set(w,e);
            }
          }
          break;
        case Heap::POST_HEAP:
          break;
        }
      }
      return v;
    }



    ///The next node to be processed.

    ///Returns the next node to be processed or \c INVALID if the
    ///priority heap is empty.
    Node nextNode() const
    {
      return !_heap->empty()?_heap->top():INVALID;
    }

    ///Returns \c false if there are nodes to be processed.

    ///Returns \c false if there are nodes to be processed
    ///in the priority heap.
    bool emptyQueue() const { return _heap->empty(); }

    ///Returns the number of the nodes to be processed.

    ///Returns the number of the nodes to be processed
    ///in the priority heap.
    int queueSize() const { return _heap->size(); }

    ///Executes the algorithm.

    ///Executes the algorithm.
    ///
    ///This method runs the %Dijkstra algorithm from the root node(s)
    ///in order to compute the shortest path to each node.
    ///
    ///The algorithm computes
    ///- the shortest path tree (forest),
    ///- the distance of each node from the root(s).
    ///
    ///\pre init() must be called and at least one root node should be
    ///added with addSource() before using this function.
    ///
    ///\note <tt>d.start()</tt> is just a shortcut of the following code.
    ///\code
    ///  while ( !d.emptyQueue() ) {
    ///    d.processNextNode();
    ///  }
    ///\endcode
    void start()
    {
      while ( !emptyQueue() ) processNextNode();
    }

    void start_Steiner(Graph::NodeMap <bool> * terminals, int number_of_terminals)
    {
      int number_reached_terminals = 0;
      while (number_reached_terminals < number_of_terminals) {
    	  processNextNode_Steiner(terminals, &number_reached_terminals);
      }
    }

    ///Executes the algorithm until the given target node is processed.

    ///Executes the algorithm until the given target node is processed.
    ///
    ///This method runs the %Dijkstra algorithm from the root node(s)
    ///in order to compute the shortest path to \c t.
    ///
    ///The algorithm computes
    ///- the shortest path to \c t,
    ///- the distance of \c t from the root(s).
    ///
    ///\pre init() must be called and at least one root node should be
    ///added with addSource() before using this function.
    void start(Node t)
    {
      while ( !_heap->empty() && _heap->top()!=t ) processNextNode();
      if ( !_heap->empty() ) {
        finalizeNodeData(_heap->top(),_heap->prio());
        _heap->pop();
      }
    }

    ///Executes the algorithm until a condition is met.

    ///Executes the algorithm until a condition is met.
    ///
    ///This method runs the %Dijkstra algorithm from the root node(s) in
    ///order to compute the shortest path to a node \c v with
    /// <tt>nm[v]</tt> true, if such a node can be found.
    ///
    ///\param nm A \c bool (or convertible) node map. The algorithm
    ///will stop when it reaches a node \c v with <tt>nm[v]</tt> true.
    ///
    ///\return The reached node \c v with <tt>nm[v]</tt> true or
    ///\c INVALID if no such node was found.
    ///
    ///\pre init() must be called and at least one root node should be
    ///added with addSource() before using this function.
    template<class NodeBoolMap>
    Node start(const NodeBoolMap &nm)
    {
      while ( !_heap->empty() && !nm[_heap->top()] ) processNextNode();
      if ( _heap->empty() ) return INVALID;
      finalizeNodeData(_heap->top(),_heap->prio());
      return _heap->top();
    }

    ///Runs the algorithm from the given source node.

    ///This method runs the %Dijkstra algorithm from node \c s
    ///in order to compute the shortest path to each node.
    ///
    ///The algorithm computes
    ///- the shortest path tree,
    ///- the distance of each node from the root.
    ///
    ///\note <tt>d.run(s)</tt> is just a shortcut of the following code.
    ///\code
    ///  d.init();
    ///  d.addSource(s);
    ///  d.start();
    ///\endcode
    void run(Node s) {
      init();
      addSource(s);
      start();
    }

    void run_Steiner(Node s, Graph::NodeMap <bool> * terminals, int number_of_terminals) {
          init();
          addSource(s);
          start_Steiner(terminals, number_of_terminals);
        }

    ///Finds the shortest path between \c s and \c t.

    ///This method runs the %Dijkstra algorithm from node \c s
    ///in order to compute the shortest path to node \c t
    ///(it stops searching when \c t is processed).
    ///
    ///\return \c true if \c t is reachable form \c s.
    ///
    ///\note Apart from the return value, <tt>d.run(s,t)</tt> is just a
    ///shortcut of the following code.
    ///\code
    ///  d.init();
    ///  d.addSource(s);
    ///  d.start(t);
    ///\endcode
    bool run(Node s,Node t) {
      init();
      addSource(s);
      start(t);
      return (*_heap_cross_ref)[t] == Heap::POST_HEAP;
    }

    ///@}

    ///\name Query Functions
    ///The results of the %Dijkstra algorithm can be obtained using these
    ///functions.\n
    ///Either \ref run(Node) "run()" or \ref init() should be called
    ///before using them.

    ///@{

    ///The shortest path to the given node.

    ///Returns the shortest path to the given node from the root(s).
    ///
    ///\warning \c t should be reached from the root(s).
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function.
    Path path(Node t) const { return Path(*G, *_pred, t); }

    ///The distance of the given node from the root(s).

    ///Returns the distance of the given node from the root(s).
    ///
    ///\warning If node \c v is not reached from the root(s), then
    ///the return value of this function is undefined.
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function.
    Value dist(Node v) const { return (*_dist)[v]; }

    ///\brief Returns the 'previous arc' of the shortest path tree for
    ///the given node.
    ///
    ///This function returns the 'previous arc' of the shortest path
    ///tree for the node \c v, i.e. it returns the last arc of a
    ///shortest path from a root to \c v. It is \c INVALID if \c v
    ///is not reached from the root(s) or if \c v is a root.
    ///
    ///The shortest path tree used here is equal to the shortest path
    ///tree used in \ref predNode() and \ref predMap().
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function.
    Arc predArc(Node v) const { return (*_pred)[v]; }

    ///\brief Returns the 'previous node' of the shortest path tree for
    ///the given node.
    ///
    ///This function returns the 'previous node' of the shortest path
    ///tree for the node \c v, i.e. it returns the last but one node
    ///of a shortest path from a root to \c v. It is \c INVALID
    ///if \c v is not reached from the root(s) or if \c v is a root.
    ///
    ///The shortest path tree used here is equal to the shortest path
    ///tree used in \ref predArc() and \ref predMap().
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function.
    Node predNode(Node v) const { return (*_pred)[v]==INVALID ? INVALID:
                                  G->source((*_pred)[v]); }

    ///\brief Returns a const reference to the node map that stores the
    ///distances of the nodes.
    ///
    ///Returns a const reference to the node map that stores the distances
    ///of the nodes calculated by the algorithm.
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function.
    const DistMap &distMap() const { return *_dist;}

    ///\brief Returns a const reference to the node map that stores the
    ///predecessor arcs.
    ///
    ///Returns a const reference to the node map that stores the predecessor
    ///arcs, which form the shortest path tree (forest).
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function.
    const PredMap &predMap() const { return *_pred;}

    ///Checks if the given node is reached from the root(s).

    ///Returns \c true if \c v is reached from the root(s).
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function.
    bool reached(Node v) const { return (*_heap_cross_ref)[v] !=
                                        Heap::PRE_HEAP; }

    ///Checks if a node is processed.

    ///Returns \c true if \c v is processed, i.e. the shortest
    ///path to \c v has already found.
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function.
    bool processed(Node v) const { return (*_heap_cross_ref)[v] ==
                                          Heap::POST_HEAP; }

    ///The current distance of the given node from the root(s).

    ///Returns the current distance of the given node from the root(s).
    ///It may be decreased in the following processes.
    ///
    ///\pre Either \ref run(Node) "run()" or \ref init()
    ///must be called before using this function and
    ///node \c v must be reached but not necessarily processed.
    Value currentDist(Node v) const {
      return processed(v) ? (*_dist)[v] : (*_heap)[v];
    }

    ///@}
  };

} //END OF NAMESPACE LEMON

#endif
