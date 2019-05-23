#include "LKH_Lib.h"

#include "LKH.h"
#include "Genetic.h"
#include "BIT.h"
#include "Heap.h"

/*
 * This file contains the main function of the program.
 */

static char *Copy(char *S);
static void CreateNodes();
static void ReadEdgeWeights(int *graph);
static void ReadGraph(int *graph, int nodes);
static void SetParameters(int runs);
static void WriteTourToArray(int *BestTour, int *tour);


GainType LKH_LOWER_DIAG_ROW(int *graph, int nodes, int runs, int *tour) {
    GainType Cost, OldOptimum;
    double Time, LastTime = GetTime();

    SetParameters(runs);
    MaxMatrixDimension = 20000;
    MergeWithTour = Recombination == IPT ? MergeWithTourIPT :
        MergeWithTourGPX2;
    ReadGraph(graph, nodes);
    AllocateStructures();
    CreateCandidateSet();
    InitializeStatistics();

    Norm = 9999;
    BestCost = PLUS_INFINITY;
    BestPenalty = CurrentPenalty = PLUS_INFINITY;
    /* Find a specified number (Runs) of local optima */

    for (Run = 1; Run <= Runs; Run++) {
        Cost = FindTour();      /* using the Lin-Kernighan heuristic */
        if (Run > 1 && !TSPTW_Makespan) {
            Cost = MergeTourWithBestTour();
        }
        if (CurrentPenalty < BestPenalty ||
            (CurrentPenalty == BestPenalty && Cost < BestCost)) {
            BestPenalty = CurrentPenalty;
            BestCost = Cost;
            RecordBetterTour();
            RecordBestTour();
        }
        OldOptimum = Optimum;
        if (!Penalty ||
            (MTSPObjective != MINMAX && MTSPObjective != MINMAX_SIZE)) {
            if (CurrentPenalty == 0 && Cost < Optimum)
                Optimum = Cost;
        } else if (CurrentPenalty < Optimum) {
            Optimum = CurrentPenalty;
        }
        if (Optimum < OldOptimum) {
            if (FirstNode->InputSuc) {
                Node *N = FirstNode;
                while ((N = N->InputSuc = N->Suc) != FirstNode);
            }
        }
        Time = fabs(GetTime() - LastTime);
        UpdateStatistics(Cost, Time);
        SRandom(++Seed);
    }
    WriteTourToArray(BestTour, tour);
    return BestCost;
}

static char *Copy(char *S)
{
    char *Buffer;

    if (!S || strlen(S) == 0)
        return 0;
    assert(Buffer = (char *) malloc(strlen(S) + 1));
    strcpy(Buffer, S);
    return Buffer;
}

static void CreateNodes()
{
    Node *Prev = 0, *N = 0;
    int i;

    if (Dimension <= 0)
        eprintf("DIMENSION is not positive (or not specified)");
    assert(NodeSet = (Node *) calloc(Dimension + 1, sizeof(Node)));
    for (i = 1; i <= Dimension; i++, Prev = N) {
        N = &NodeSet[i];
        if (i == 1)
            FirstNode = N;
        else
            Link(Prev, N);
        N->Id = i;
        if (MergeTourFiles >= 1)
            assert(N->MergeSuc =
                   (Node **) calloc(MergeTourFiles, sizeof(Node *)));
        N->Earliest = 0;
        N->Latest = INT_MAX;
    }
    Link(N, FirstNode);
}

static void ReadEdgeWeights(int *graph)
{
    Node *Ni;
    int i, j, W;
    // CheckSpecificationPart();
    if (!FirstNode)
        CreateNodes();

   assert(CostMatrix =
          (int *) calloc((size_t) Dimension * (Dimension - 1) / 2,
                         sizeof(int)));
   Ni = FirstNode->Suc;
   do {
       Ni->C =
           &CostMatrix[(size_t) (Ni->Id - 1) * (Ni->Id - 2) / 2] - 1;
   }
   while ((Ni = Ni->Suc) != FirstNode);

    int weight_pos = 0;
    switch (WeightFormat) {
    case LOWER_ROW:
        for (i = 2; i <= Dim; i++) {
            for (j = 1; j < i; j++, weight_pos++) {
                W = round(Scale * graph[weight_pos]);
                if (Penalty && W < 0)
                    eprintf("EDGE_WEIGHT_SECTION: Negative weight");
                NodeSet[i].C[j] = W;
            }
        }
        break;
    case LOWER_DIAG_ROW:
        for (i = 1; i <= Dim; i++) {
            for (j = 1; j <= i; j++, weight_pos++) {
                if (j == i)
                    continue;
                W = round(Scale * graph[weight_pos]);
                if (W > INT_MAX / 2 / Precision)
                    W = INT_MAX / 2 / Precision;
                if (Penalty && W < 0)
                    eprintf("EDGE_WEIGHT_SECTION: Negative weight");
                if (j != i)
                    NodeSet[i].C[j] = W;
            }
        }
        break;
    }
}

static void ReadGraph(int *graph, int nodes) {
   int i, j, K;

   FreeStructures();
   FirstNode = 0;
   WeightType = EXPLICIT;
   WeightFormat = LOWER_DIAG_ROW;
   ProblemType = TSP;
   Asymmetric = 0;
   DimensionSaved = Dim = Dimension = nodes;
   CoordType = NO_COORDS;
   Name = Copy("TSP");
   Type = EdgeWeightType = EdgeWeightFormat = 0;
   EdgeDataFormat = NodeCoordType = DisplayDataType = 0;
   Distance = Distance_EXPLICIT;
   C = 0;
   c = 0;
   DistanceLimit = DBL_MAX;
   Scale = 1;

   ReadEdgeWeights(graph);

   Swaps = 0;

   /* Adjust parameters */
   if (Seed == 0)
       Seed = (unsigned) time(0);
   if (Precision == 0)
       Precision = 100;
   if (InitialStepSize == 0)
       InitialStepSize = 1;
   if (MaxSwaps < 0)
       MaxSwaps = Dimension;
   if (KickType > Dimension / 2)
       KickType = Dimension / 2;
   if (Runs == 0)
       Runs = 10;
   if (MaxCandidates > Dimension - 1)
       MaxCandidates = Dimension - 1;
   if (ExtraCandidates > Dimension - 1)
       ExtraCandidates = Dimension - 1;
   if (Scale < 1)
       Scale = 1;
   if (SubproblemSize >= Dimension)
       SubproblemSize = Dimension;
   else if (SubproblemSize == 0) {
       if (AscentCandidates > Dimension - 1)
           AscentCandidates = Dimension - 1;
       if (InitialPeriod < 0) {
           InitialPeriod = Dimension / 2;
           if (InitialPeriod < 100)
               InitialPeriod = 100;
       }
       if (Excess < 0)
           Excess = 1.0 / DimensionSaved * Salesmen;
       if (MaxTrials == -1)
           MaxTrials = Dimension;
       HeapMake(Dimension);
   }
   if (POPMUSIC_MaxNeighbors > Dimension - 1)
       POPMUSIC_MaxNeighbors = Dimension - 1;
   if (POPMUSIC_SampleSize > Dimension)
       POPMUSIC_SampleSize = Dimension;
   Depot = &NodeSet[MTSPDepot];
   TSPTW_Makespan = 0;

   if (Penalty && (SubproblemSize > 0 || SubproblemTourFile))
       eprintf("Partitioning not implemented for constrained problems");
   Depot->DepotId = 1;
   for (i = Dim + 1; i <= DimensionSaved; i++)
       NodeSet[i].DepotId = i - Dim + 1;
   if (Dimension != DimensionSaved) {
       NodeSet[Depot->Id + DimensionSaved].DepotId = 1;
       for (i = Dim + 1; i <= DimensionSaved; i++)
           NodeSet[i + DimensionSaved].DepotId = i - Dim + 1;
   }
   if (Scale < 1)
       Scale = 1;
   else {
       Node *Ni = FirstNode;
       do {
           Ni->Earliest *= Scale;
           Ni->Latest *= Scale;
           Ni->ServiceTime *= Scale;
       } while ((Ni = Ni->Suc) != FirstNode);
       ServiceTime *= Scale;
       RiskThreshold *= Scale;
       if (DistanceLimit != DBL_MAX)
           DistanceLimit *= Scale;
   }
   if (ServiceTime != 0) {
       for (i = 1; i <= Dim; i++)
           NodeSet[i].ServiceTime = ServiceTime;
       Depot->ServiceTime = 0;
   }
   if (CostMatrix == 0 && Dimension <= MaxMatrixDimension &&
       Distance != 0 && Distance != Distance_1
       && Distance != Distance_LARGE && Distance != Distance_ATSP
       && Distance != Distance_MTSP && Distance != Distance_SPECIAL) {
       Node *Ni, *Nj;
       assert(CostMatrix =
              (int *) calloc((size_t) Dim * (Dim - 1) / 2, sizeof(int)));
       Ni = FirstNode->Suc;
       do {
           Ni->C =
               &CostMatrix[(size_t) (Ni->Id - 1) * (Ni->Id - 2) / 2] - 1;
           if (ProblemType != HPP || Ni->Id <= Dim)
               for (Nj = FirstNode; Nj != Ni; Nj = Nj->Suc)
                   Ni->C[Nj->Id] = Fixed(Ni, Nj) ? 0 : Distance(Ni, Nj);
           else
               for (Nj = FirstNode; Nj != Ni; Nj = Nj->Suc)
                   Ni->C[Nj->Id] = 0;
       }
       while ((Ni = Ni->Suc) != FirstNode);
       c = 0;
       WeightType = EXPLICIT;
   }
   C = WeightType == EXPLICIT ? C_EXPLICIT : C_FUNCTION;
   D = WeightType == EXPLICIT ? D_EXPLICIT : D_FUNCTION;
   if (Precision > 1 && CostMatrix) {
       for (i = 2; i <= Dim; i++) {
           Node *N = &NodeSet[i];
           for (j = 1; j < i; j++)
               if (N->C[j] * Precision / Precision != N->C[j])
                   eprintf("PRECISION (= %d) is too large", Precision);
       }
   }
   if (SubsequentMoveType == 0) {
       SubsequentMoveType = MoveType;
       SubsequentMoveTypeSpecial = MoveTypeSpecial;
   }
   K = MoveType >= SubsequentMoveType || !SubsequentPatching ?
       MoveType : SubsequentMoveType;
   if (PatchingC > K)
       PatchingC = K;
   if (PatchingA > 1 && PatchingA >= PatchingC)
       PatchingA = PatchingC > 2 ? PatchingC - 1 : 1;
   if (NonsequentialMoveType == -1 ||
       NonsequentialMoveType > K + PatchingC + PatchingA - 1)
       NonsequentialMoveType = K + PatchingC + PatchingA - 1;
   if (PatchingC >= 1) {
       BestMove = BestSubsequentMove = BestKOptMove;
       if (!SubsequentPatching && SubsequentMoveType <= 5) {
           MoveFunction BestOptMove[] =
               { 0, 0, Best2OptMove, Best3OptMove,
               Best4OptMove, Best5OptMove
           };
           BestSubsequentMove = BestOptMove[SubsequentMoveType];
       }
   } else {
       MoveFunction BestOptMove[] = { 0, 0, Best2OptMove, Best3OptMove,
           Best4OptMove, Best5OptMove
       };
       BestMove = MoveType <= 5 ? BestOptMove[MoveType] : BestKOptMove;
       BestSubsequentMove = SubsequentMoveType <= 5 ?
           BestOptMove[SubsequentMoveType] : BestKOptMove;
   }
   if (MoveTypeSpecial)
       BestMove = BestSpecialOptMove;
   if (SubsequentMoveTypeSpecial)
       BestSubsequentMove = BestSpecialOptMove;

   LastLine = 0;
}

static void SetParameters(int runs) {
   ProblemFileName = PiFileName = InputTourFileName =
       OutputTourFileName = TourFileName = 0;
   CandidateFiles = MergeTourFiles = 0;
   AscentCandidates = 50;
   BackboneTrials = 0;
   Backtracking = 0;
   BWTSP_B = 0;
   BWTSP_Q = 0;
   BWTSP_L = INT_MAX;
   CandidateSetSymmetric = 0;
   CandidateSetType = ALPHA;
   Crossover = ERXT;
   DelaunayPartitioning = 0;
   DelaunayPure = 0;
   DemandDimension = 1;
   Excess = -1;
   ExtraCandidates = 0;
   ExtraCandidateSetSymmetric = 0;
   ExtraCandidateSetType = QUADRANT;
   Gain23Used = 1;
   GainCriterionUsed = 1;
   GridSize = 1000000.0;
   InitialPeriod = -1;
   InitialStepSize = 0;
   InitialTourAlgorithm = WALK;
   InitialTourFraction = 1.0;
   KarpPartitioning = 0;
   KCenterPartitioning = 0;
   KMeansPartitioning = 0;
   Kicks = 1;
   KickType = 0;
   MaxBreadth = INT_MAX;
   MaxCandidates = 5;
   MaxPopulationSize = 0;
   MaxSwaps = -1;
   MaxTrials = -1;
   MoorePartitioning = 0;
   MoveType = 5;
   MoveTypeSpecial = 0;
   MTSPDepot = 1;
   MTSPMinSize = 1;
   MTSPMaxSize = -1;
   MTSPObjective = -1;
   NonsequentialMoveType = -1;
   Optimum = MINUS_INFINITY;
   PatchingA = 2;
   PatchingC = 3;
   PatchingAExtended = 0;
   PatchingARestricted = 0;
   PatchingCExtended = 0;
   PatchingCRestricted = 0;
   Precision = 100;
   POPMUSIC_InitialTour = 0;
   POPMUSIC_MaxNeighbors = 5;
   POPMUSIC_SampleSize = 10;
   POPMUSIC_Solutions = 50;
   POPMUSIC_Trials = 1;
   Recombination = IPT;
   RestrictedSearch = 1;
   RohePartitioning = 0;
   Runs = runs;
   Salesmen = 1;
   Scale = -1;
   Seed = 1;
   SierpinskiPartitioning = 0;
   StopAtOptimum = 1;
   Subgradient = 1;
   SubproblemBorders = 0;
   SubproblemsCompressed = 0;
   SubproblemSize = 0;
   SubsequentMoveType = 0;
   SubsequentMoveTypeSpecial = 0;
   SubsequentPatching = 1;
   TimeLimit = DBL_MAX;
   TraceLevel = 1;
   TSPTW_Makespan = 0;
   s1_Gain23 = 0;
   OldReversed_Gain23 = 0;
}

static void WriteTourToArray(int *BestTour, int *Tour) {
   int i, j, n, Forward;
   int out_i = 0;
   n = DimensionSaved;
   for (i = 1; i < n && BestTour[i] != 1; i++);
   Forward = Asymmetric ||
       BestTour[i < n ? i + 1 : 1] < BestTour[i > 1 ? i - 1 : Dimension];
   for (j = 1; j <= n; j++) {
       if (BestTour[i] <= n)
           Tour[out_i++] = BestTour[i];
       if (Forward) {
           if (++i > n)
               i = 1;
       } else if (--i < 1)
           i = n;
   }
}
