#include "algorithm.h"

Algorithm::Algorithm(Graph *graph)
{
    this->graph = graph;
    start_node_ind = -1;
    chosenAlgo = Algorithms::None;
    init_ready = false;
    adj_ind_in_u = 0;
    u = -1;
    time = 0;
    timer = new QTimer();
    r = 0;
    in_dfs_visit = false;

    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &Algorithm::stepAlgorithm);
}


// BEÁLLÍTÁSOK

void Algorithm::selectAlgorithm(Algorithm::Algorithms algo)
{
    chosenAlgo = algo;
    init_ready = false;
}

void Algorithm::selectStartNode(int s_ind)
{
    start_node_ind = s_ind;
    init_ready = false;
}


// VEZÉRLÉS

void Algorithm::startAlgorithm()
{
    if (!init_ready) init();
    timer->start();
}

void Algorithm::pauseAlgrotithm()
{
    timer->stop();
}

void Algorithm::reset()
{
    timer->stop();
    init_ready = false;
    adj_ind_in_u = 0;
    u = -1;
    time = 0;
    r = 0;
    in_dfs_visit = false;
    for(int i=0; i<graph->getSize(); i++) {
        int id = graph->getId(i);
        emit nodeStateChange(NodeType::BaseNode, id);
            for (int j=0; j<graph->getAdjNum(i); j++) {
                int adj_id = graph->getId(graph->getAdjIndexInNodes(i,j));
                emit edgeStateChange(EdgeType::BaseEdge, id, adj_id);
            }
    }
}

bool Algorithm::getInitState()
{
    return init_ready;
}

Algorithm::Algorithms Algorithm::getChosenAlgo()
{
    return chosenAlgo;
}


// ALGORITMUS

void Algorithm::init()                                  // ( 1 )
{
    graph->sortNodes();
    bool can_start = false;
    steps.clear();
    queue.clear();
    int n = graph->getSize();
    distances.resize(n);
    parents.resize(n);
    nodeTypes.resize(n);
    edgeTypes.resize(n);

    switch (chosenAlgo) {
        case Szelessegi:

        // gráf ellenőrzése - súlyozatlan
        if (graph->getWeighted()) emit noWeights();

        else {

            for(int i=0; i<n; i++) {                                             // első ciklus - alapértékek
                distances[i] = INT32_MAX;
                parents[i] = -1;

                nodeTypes[i] = BaseNode;
                emit nodeStateChange(NodeType::BaseNode, graph->getId(i));
                edgeTypes[i].resize(n);
                for(int j = 0; j<graph->getAdjNum(i); j++) {
                    int index_in_nodes = graph->getAdjIndexInNodes(i,j);
                    edgeTypes[i][index_in_nodes] = BaseEdge;
                    emit edgeStateChange(BaseEdge, graph->getId(i), graph->getId(index_in_nodes));
                }
            }
            can_start = true;

        }
        break;

    case Melysegi:

        // gráf ellenőrzése - súlyozatlan, irányított
        if (graph->getWeighted()) emit noWeights();
        if (!graph->getDirected()) emit needsToBeDirected();

        if (!graph->getWeighted() && graph->getDirected()) {
            time = 0;
            r = 0;
            in_dfs_visit = false;
            adj_ind_in_u = -1;
            prev_u = -1;
            adj_ind_in_us.clear();
            adj_ind_in_us.resize(n);

            discovery_time.clear();
            finishing_time.clear();
            discovery_time.resize(n);
            finishing_time.resize(n);

            for(int i=0; i<n; i++) {                                             // első ciklus - alapértékek

                parents[i] = -1;                                    // az algoritmus szerint nem kellene, de a visszalépéshez kell
                nodeTypes[i] = BaseNode;
                emit nodeStateChange(NodeType::BaseNode, graph->getId(i));
                edgeTypes[i].resize(n);
                for(int j = 0; j<graph->getAdjNum(i); j++) {
                    int index_in_nodes = graph->getAdjIndexInNodes(i,j);
                    edgeTypes[i][index_in_nodes] = BaseEdge;
                    emit edgeStateChange(BaseEdge, graph->getId(i), graph->getId(index_in_nodes));
                }
            }
            can_start = true;
        }

        break;

    case Dijkstra:

        // gráf ellenőrzése - súlyozott és nemnegatív súlyok
        if (!graph->getWeighted()) emit needWeights();
        else if (!graph->checkAllEdgesNonnegative()) emit needOnlyNonnegativeEdges();

        // ha futtatható a gráfon az algoritmus, elindíthatjuk
        if(graph->getWeighted() && graph->checkAllEdgesNonnegative()) {

            for(int i=0; i<n; i++) {                                            // első ciklus - alapértékek
                distances[i] = INT32_MAX;
                parents[i] = -1;

                nodeTypes[i] = BaseNode;
                emit nodeStateChange(NodeType::BaseNode, graph->getId(i));
                edgeTypes[i].resize(n);
                for(int j=0; j < graph->getAdjNum(i); j++) {
                    int index_in_nodes = graph->getAdjIndexInNodes(i,j);
                    edgeTypes[i][index_in_nodes] = EdgeType::BaseEdge;
                    emit edgeStateChange(EdgeType::BaseEdge, graph->getId(i), graph->getId(index_in_nodes));
                }
            }
            can_start = true;
        }
        break;

    case Prim:

        // gráf ellenőrzése - élsúlyozott, összefüggő, irányítatlan
        if (!graph->getWeighted()) emit needWeights();
        if (graph->getDirected()) emit needsToBeUndirected();
        if (!graph->checkConnectivity()) emit needsToBeConnected();

        if (graph->getWeighted() && !graph->getDirected() && graph->checkConnectivity()) {

            for(int i=0; i<n; i++) {                                        // első ciklus - alapértékek
                distances[i] = INT32_MAX;
                parents[i] = -1;

                nodeTypes[i] = BaseNode;
                emit nodeStateChange(NodeType::BaseNode, graph->getId(i));
                edgeTypes[i].resize(n);

                for(int j=0; j < graph->getAdjNum(i); j++) {
                    int index_in_nodes = graph->getAdjIndexInNodes(i,j);
                    edgeTypes[i][index_in_nodes] = EdgeType::BaseEdge;
                    emit edgeStateChange(EdgeType::BaseEdge, graph->getId(i), graph->getId(index_in_nodes));
                }
            }
            can_start = true;
        }
        break;

    default:
        break;
    }
    if (can_start) {
        if (start_node_ind > -1 && start_node_ind < graph->getSize()) {
            initNode();
            init_ready = true;
            sig = SignalForStuki::Init;
            addState();
            emit initReady(start_node_ind);
        }
    }
}

void Algorithm::initNode()                              // ( 2 )
{
    switch (chosenAlgo) {

    case Szelessegi:

        distances[start_node_ind] = 0;                              // kezdő csúcshoz az út 0

        nodeTypes[start_node_ind] = ReachedButNotProcessed;         // color = grey
        emit nodeStateChange(ReachedButNotProcessed, graph->getId(start_node_ind));

        queue.append(start_node_ind);                               // a sorba csak a kezdőcsúcs kerül
        emit queueChanged(queueToVector());

        break;


    case Dijkstra:

        distances[start_node_ind] = 0;                              // kezdő csúcshoz az út 0
        for(int i=0; i<graph->getSize(); i++) {                     // minimum prioritásos sor feltöltése a gráf csúcsaival (kivéve a kezdőcsúcsot)
            if (i != start_node_ind)
                queue.append(i);
        }

        emit queueChanged(queueToVectorMin());

        u = start_node_ind;
        emit nodeStateChange(NodeType::ExamineAdj, graph->getId(u));
        break;


    case Prim:
        distances[start_node_ind] = 0;                              // kezdő csúcs 0 költséggel kerül be

        for(int i=0; i<graph->getSize(); i++) {                     // minimum prioritásos sor feltöltése a gráf csúcsaival (kivéve a kezdőcsúcsot)
            if (i != start_node_ind)
                queue.append(i);
        }

        emit queueChanged(queueToVectorMin());
        u = start_node_ind;
        emit nodeStateChange(NodeType::ExamineAdj, graph->getId(u));
        break;

    default:
        break;

    }

}

bool Algorithm::stepAlgorithm()                         // ( 3 )
{
    if (chosenAlgo == None) return false;
    if (!init_ready) {
        init();
        return true;
    }
    bool ended = false;
    emit step_start();

    switch (chosenAlgo) {
        case Szelessegi:
            ended = stepSzelessegi();
        break;

    case Melysegi:
            ended = stepMelysegi();
        break;

    case Dijkstra:
            ended = stepDijkstra();
        break;

    case Prim:
            ended = stepPrim();
        break;
    default:
        break;
    }
    addState();
    if (ended) {
        for(int i=0; i<graph->getSize(); i++) {
            emit nodeStateChange(ProcessedNode, graph->getId(i));
            for(int j=0; j<graph->getSize(); j++) {
                if (i != j && edgeTypes[i][j] == EdgeType::BaseEdge) {
                    edgeTypes[i][j] = EdgeType::NotNeededEdge;
                    emit edgeStateChange(EdgeType::NotNeededEdge, graph->getId(i), graph->getId(j));
                }
            }
        }
        timer->stop();
        emit algorithmEnded();
        return false;
    }
    return true;
}

bool Algorithm::stepDijkstra()
{
    bool ended = !(distances[u] < INT32_MAX && !queue.isEmpty());                           // külső ciklus feltétele

    if (adj_ind_in_u > 0) {                                                                 // az előző lépésben vizsgált gyereket visszaállítjuk ( ha volt )
        int prev_ind = graph->getAdjIndexInNodes(u,adj_ind_in_u-1);
        int prev_id =  graph->getId(prev_ind);
        emit nodeStateChange(nodeTypes.at(prev_ind), prev_id);
        emit edgeStateChange(edgeTypes[u][prev_ind], graph->getId(u), prev_id);
    }


    if (!ended) {                                                                           // külső ciklusmag

        if (adj_ind_in_u < graph->getAdjNum(u)) {                                           // belső ciklusfeltétel (az összes szomszédot megnézzük)

            int adj_index = graph->getAdjIndexInNodes(u,adj_ind_in_u);                      // aktuálisan vizsgált szomszéd
            emit nodeStateChange(ExaminedNode, graph->getId(adj_index));
            emit edgeStateChange(ExaminedEdge, graph->getId(u), graph->getId(adj_index));

            int newDist = distances[u] + graph->getAdjWeight(u,adj_ind_in_u);               // elérési távolság az aktuális csúcson keresztül
            if (distances[adj_index] > newDist) {                                           // jobb utat találtunk-e?


                if (parents[adj_index] != -1 && (graph->getDirected() || parents[u] != adj_index)) {    // ha volt már ide út, akkor az nem kell
                    edgeTypes[parents[adj_index]][adj_index] = EdgeType::NotNeededEdge;
                    if (!graph->getDirected()) {
                        edgeTypes[adj_index][parents[adj_index]] = EdgeType::NotNeededEdge;
                    }
                    emit edgeStateChange(EdgeType::NotNeededEdge, graph->getId(parents[adj_index]), graph->getId(adj_index));
                }

                parents[adj_index] = u;                                                     // új értékek
                distances[adj_index] = newDist;
                emit parentChanged(adj_index, graph->getName(graph->getId(u)));
                emit distChanged(adj_index, newDist);
                edgeTypes[u][adj_index] = EdgeType::NeededEdge;
                if (!graph->getDirected()) edgeTypes[adj_index][u] = EdgeType::NeededEdge;

                emit queueChanged(queueToVectorMin());                                      // Q.adjust miatt (helyes sorrend jelenjen meg)

                // stuki kirajzolásához
                emit ifTrue();
                sig = True;

            } else if ((graph->getDirected() || parents[u] != adj_index)){                  // az újonnan talált él nem jobb az eddiginél, ezért nem kell

                edgeTypes[u][adj_index] = EdgeType::NotNeededEdge;
                if (!graph->getDirected()) edgeTypes[adj_index][u] = EdgeType::NotNeededEdge;

                // stuki kirajzolásához
                emit ifFalse();
                sig = False;

            } else {
                // stuki kirajzolásához
                emit ifFalse();
                sig = False;
            }
            adj_ind_in_u++;                                                                 // a belső ciklust léptetjük (következő szomszédot vizsgáljuk)

        } else {                                                                            // belső ciklus utáni értékadás

            adj_ind_in_u = 0;                                                               // a szomszédokat az elejéről nézzük
            emit nodeStateChange(ProcessedNode, graph->getId(u));                           // végeztünk az aktuális csúcs kiterjesztésével, ehhez már a legjobb utat ismerjü
            nodeTypes[u] = ProcessedNode;

            u = remMin(queue);                                                              // kivesszük a sorból a minimumot (ehhez is a legjobb utat ismerjük)
            emit queueChanged(queueToVectorMin());
            emit nodeStateChange(ExamineAdj, graph->getId(u));

            // stuki kirajzolásához
            emit outerLoop();
            sig = Outer;
        }
    }
    return ended;
}

bool Algorithm::stepSzelessegi()
{
    bool ended = !(!queue.isEmpty() || (graph->getAdjNum(u) != 0 && adj_ind_in_u < graph->getAdjNum(u)));       // külső ciklus feltétel + lehet, hogy a belső ciklusban átmenetileg üres a sor, ezért meg kell vizsgálni, hogy a belső ciklusban vagyunk-e

    if (adj_ind_in_u > 0) {                                                                 // az előző lépésben vizsgált gyereket visszaállítjuk ( ha volt )
        int prev_ind = graph->getAdjIndexInNodes(u,adj_ind_in_u-1);
        int prev_id =  graph->getId(prev_ind);
        emit nodeStateChange(nodeTypes.at(prev_ind), prev_id);
        emit edgeStateChange(edgeTypes[u][prev_ind], graph->getId(u), prev_id);
    }


    if (!ended) {                                                                           // külső ciklusmag

        if (u == -1 || adj_ind_in_u == graph->getAdjNum(u)) {       // még egyszer sem futott le a külső ciklusmag vagy épp vége lett a belső ciklusnak

            if (u != -1) {                                          // ha nem az első futás eleje, akkor valamelyik vége, a külső ciklus utolsó értékadása jön
                adj_ind_in_u = 0;                                   // a szomszédokat az elejéről nézzük

                emit nodeStateChange(ProcessedNode, graph->getId(u));
                nodeTypes[u] = ProcessedNode;                       // amit eddig néztünk, feldolgoztuk (color = black)
            }

            u = queue.first();                                      // külső ciklusmag első értékadása
            queue.removeFirst();
            emit queueChanged(queueToVector());
            emit nodeStateChange(ExamineAdj, graph->getId(u));

            // stuki kirajzolásához
            emit outerLoop();
            sig = Outer;

        } else {                                                                        // belső ciklusmag

            int adj_index = graph->getAdjIndexInNodes(u,adj_ind_in_u);          // aktuálisan vizsgált szomszéd
            emit nodeStateChange(ExaminedNode, graph->getId(adj_index));
            emit edgeStateChange(ExaminedEdge, graph->getId(u), graph->getId(adj_index));

            if (distances[adj_index] == INT32_MAX) {                            // eddig ismeretlen csúcsot találtunk?

                distances[adj_index] = distances[u] + 1;                        // új értékek
                parents[adj_index] = u;
                emit distChanged(adj_index, distances[u] + 1);
                emit parentChanged(adj_index, graph->getName(graph->getId(u)));
                nodeTypes[adj_index] = ReachedButNotProcessed;                  // color = grey
                edgeTypes[u][adj_index] = NeededEdge;
                if (!graph->getDirected()) edgeTypes[adj_index][u] = NeededEdge;

                queue.append(adj_index);                                        // hozzávesszü ezt a szomszédot a feldolgozandó csúcsokhoz
                emit queueChanged(queueToVector());

                // stuki kirajzolásához
                emit ifTrue();
                sig = True;

            } else if ((graph->getDirected() || parents[u] != adj_index)) {     // ha korábban már megtalált csúcs, akkor erre az élre nincs szükségünk

                edgeTypes[u][adj_index] = NotNeededEdge;
                if (!graph->getDirected()) edgeTypes[adj_index][u] = NotNeededEdge;

                // stuki kirajzolásához
                emit ifFalse();
                sig = False;
            } else {
                // stuki kirajzolásához
                emit ifFalse();
                sig = False;
            }

            adj_ind_in_u++;                                                     // a belső ciklust léptetjük (következő szomszédot vizsgáljuk)
        }
    }
    return ended;
}

bool Algorithm::stepPrim()
{
    bool ended = !(!queue.isEmpty()); /*||                                                  // külső ciklus feltétele
              (graph->getAdjNum(u) != 0 && adj_ind_in_u < graph->getAdjNum(u)*//* && queue.indexOf(graph->getAdjIndexInNodes(u,adj_ind_in_u)) != -1*//*)*/

    if (adj_ind_in_u > 0) {                                                                 // az előző lépésben vizsgált gyereket visszaállítjuk ( ha volt )
        int prev_ind = graph->getAdjIndexInNodes(u,adj_ind_in_u-1);
        int prev_id =  graph->getId(prev_ind);
        emit nodeStateChange(nodeTypes.at(prev_ind), prev_id);
        emit edgeStateChange(edgeTypes[u][prev_ind], graph->getId(u), prev_id);
    }

    if (!ended) {                                                                           // külső ciklusmag

        if (adj_ind_in_u < graph->getAdjNum(u)) {                                           // belső ciklusfeltétel (az összes szomszédot megnézzük)

            int adj_index = graph->getAdjIndexInNodes(u,adj_ind_in_u);                      // aktuálisan vizsgált szomszéd
            emit nodeStateChange(ExaminedNode, graph->getId(adj_index));
            emit edgeStateChange(ExaminedEdge, graph->getId(u), graph->getId(adj_index));

            int newWeight = graph->getAdjWeight(u, adj_ind_in_u);
            if (distances[adj_index] > newWeight && queue.indexOf(adj_index) != -1) {       // jobb költségű-e az új él és benne van-e még a min. prior. sorban?

                if (parents[adj_index] != -1 && parents[u] != adj_index) {                  // ha volt már ide út, akkor az nem kell
                    edgeTypes[parents[adj_index]][adj_index] = EdgeType::NotNeededEdge;
                    edgeTypes[adj_index][parents[adj_index]] = EdgeType::NotNeededEdge;
                    emit edgeStateChange(EdgeType::NotNeededEdge, graph->getId(parents[adj_index]), graph->getId(adj_index));
                }

                parents[adj_index] = u;                                                     // új értékek
                distances[adj_index] = newWeight;
                emit parentChanged(adj_index, graph->getName(graph->getId(u)));
                emit distChanged(adj_index, newWeight);
                edgeTypes[u][adj_index] = NeededEdge;
                edgeTypes[adj_index][u] = NeededEdge;

                emit queueChanged(queueToVectorMin());                                      // Q.adjust miatt (helyes sorrend jelenjen meg)

                // stuki kirajzolásához
                emit ifTrue();
                sig = True;

            } else if (parents[u] != adj_index) {                                           // az újonnan talált él nem jobb az eddiginél, ezért nem kell

                edgeTypes[u][adj_index] = NotNeededEdge;
                edgeTypes[adj_ind_in_u][u] = NotNeededEdge;

                // stuki kirajzolásához
                emit ifFalse();
                sig = False;
            } else {
                //stuki kirajzolásához
                emit ifFalse();
                sig = False;
            }

            adj_ind_in_u++;                                                                 // a belső ciklust léptetjük (következő szomszédot vizsgáljuk)

        } else {                                                                            // belső ciklus utáni értékadás
            adj_ind_in_u = 0;                                                               // a szomszédokat az elejéről nézzük
            emit nodeStateChange(ProcessedNode, graph->getId(u));
            nodeTypes[u] = ProcessedNode;

            u = remMin(queue);                                                              // kivesszük a sorból a minimumot (könnyűél - a minimális feszítőfa (MST) része)
            emit queueChanged(queueToVectorMin());
            emit nodeStateChange(ExamineAdj, graph->getId(u));

            // stuki kirajzolásához
            emit outerLoop();
            sig = Outer;
        }
    }
    return ended;
}

bool Algorithm::stepMelysegi()
{
    bool ended = (r >= graph->getSize());

    if (prev_u != -1 && adj_ind_in_us[prev_u] > 0) {
        int prev_ind = graph->getAdjIndexInNodes(prev_u, adj_ind_in_us[prev_u]-1);
        int prev_id =  graph->getId(prev_ind);
        emit edgeStateChange(edgeTypes[prev_u][prev_ind], graph->getId(prev_u), prev_id);
        if (prev_ind != u)
            emit nodeStateChange(nodeTypes.at(prev_ind), graph->getId(prev_ind));
    }

    if (!ended) {
        if (!in_dfs_visit) {                            // DFS fv ciklusmagja

            while (r < graph->getSize() && nodeTypes[r] != BaseNode)           // a false ágon lévő skip miatt
                r++;
            ended = r >= graph->getSize();
            if (!ended) {             // jártunk-e már ennél a csúcsnál?
                parents[r] = -1;
                u = r;
                emit nodeStateChange(ExamineAdj, graph->getId(u));
                for(int i=0; i<graph->getSize(); i++) {
                    adj_ind_in_us[i] = -1;
                }
                in_dfs_visit = true;                    // mostantól rekurzív fv-hívás
//                stepMelysegiVisit();

                // stuki kirajzolásához
                emit outerLoop();
                sig = Outer;
            } else {

            }

        } else {                                        // DFS visit fv
            stepMelysegiVisit();
        }
    }

    return ended;
}

void Algorithm::stepMelysegiVisit()
{
    if (adj_ind_in_us[u] == -1) {                                                   // értékadás a ciklus előtt
        discovery_time[u] = ++time;
        emit discoveryFinishChanged(u, discovery_time[u], finishing_time[u]);
        nodeTypes[u] = ReachedButNotProcessed;                                  // color = grey;
        if (parents[u] != -1) emit nodeStateChange(nodeTypes[parents[u]], graph->getId(parents[u]));
        emit nodeStateChange(ExamineAdj, graph->getId(u));
        adj_ind_in_us[u]++;

        // stuki kirajzolásához
        emit melysegiVisitFirst();
        sig = First;

    } else if (adj_ind_in_us[u] > -1 && adj_ind_in_us[u] < graph->getAdjNum(u)) {       // cikluson belül

        int adj_index = graph->getAdjIndexInNodes(u, adj_ind_in_us[u]);
        emit nodeStateChange(ExaminedNode, graph->getId(adj_index));
        emit edgeStateChange(ExaminedEdge, graph->getId(u), graph->getId(adj_index));

        if (nodeTypes[adj_index] == BaseNode) {                                 // fa-él

            parents[adj_index] = u;
            emit parentChanged(adj_index, graph->getName(graph->getId(u)));
            edgeTypes[u][adj_index] = EdgeType::NeededEdge;
//            emit edgeStateChange(NeededEdge, graph->getId(u), graph->getId(adj_index));

            // stuki kirajzolásához
            emit ifTrue();
            sig = True;


            prev_u = u;                                 // rekurzió
            u = adj_index;

        } else {

            if (nodeTypes[adj_index] == ReachedButNotProcessed) {
                edgeTypes[u][adj_index] = BackEdge;                             // vissza-él
            } else if (nodeTypes[adj_index] == ProcessedNode) {
                if (discovery_time[u] < discovery_time[adj_index]) {
                    edgeTypes[u][adj_index] = ForwardEdge;                      // előre-él
                } else if (discovery_time[u] > discovery_time[adj_index]) {
                    edgeTypes[u][adj_index] = CrossEdge;                        // kereszt-él
                }
            }
            prev_u = u;

            // stuki kirajzolásához
            emit ifFalse();
            sig = False;

        }
        adj_ind_in_us[prev_u]++;

    } else {                                                                    // ciklus utáni értékadás
        finishing_time[u] = ++time;
        emit discoveryFinishChanged(u, discovery_time[u], finishing_time[u]);
        nodeTypes[u] = ProcessedNode;
        emit nodeStateChange(ProcessedNode, graph->getId(u));
        u = parents[u];                                                     // visszalépünk a rekurzióban

        if (u == -1) {                                             // vége a rekurziónak
            in_dfs_visit = false;                                           // a legelső ciklust léptetjük
            r++;
        } else {
            emit nodeStateChange(ExamineAdj, graph->getId(u));
        }
        // stuki kirajzolásához
        emit melysegiVisitLast();
        sig = Last;
    }
}

bool Algorithm::stepBackAlgorithm()
{
    emit step_start();
    if (steps.isEmpty() || steps.length() == 1) return false;
    steps.pop();                                                // utolsó állapot, nem kell - ami épp látszik
    AlgorithmState state = steps.top();


    for(int i=0; i< graph->getSize(); i++) {

        if (chosenAlgo != Melysegi) {
            if (distances.at(i) != state.distances.at(i))
                emit distChanged(i, state.distances.at(i));

        } else {
            if (discovery_time.at(i) != state.discovery_time.at(i) || finishing_time.at(i) != state.finishing_time.at(i))
                emit discoveryFinishChanged(i, state.discovery_time.at(i), state.finishing_time.at(i));
        }
        if (parents.at(i) != state.parents.at(i)) {
            QChar n = state.parents.at(i) == -1 ? QChar() : graph->getName(graph->getId(state.parents.at(i)));
            emit parentChanged(i, n);
        }
    }
    sig = state.sig;
    switch (state.sig) {
    case Algorithm::Init:
        emit initReady(-1);
        break;
    case Algorithm::Outer:
        emit outerLoop();
        break;
    case Algorithm::True:
        emit ifTrue();
        break;
    case Algorithm::False:
        emit ifFalse();
        break;
    case Algorithm::First:
        emit melysegiVisitFirst();
        break;
    case Algorithm::Last:
        emit melysegiVisitLast();
        break;

    }

    int i=0;
    bool same = true;
    same = queue.length() == state.queue.length();
    while (i < queue.length() && same) {
        same = queue.at(i) == state.queue.at(i);
        i++;
    }
    distances = state.distances;
    parents = state.parents;
    queue = state.queue;
    if (!same) {
        if (chosenAlgo == Szelessegi) emit queueChanged(queueToVector());
        else if (chosenAlgo != Melysegi) emit queueChanged(queueToVectorMin());
    }
    u = state.u;
    adj_ind_in_u = state.adj_ind_in_u;
    adj_ind_in_us = state.adj_ind_in_us;
    nodeTypes = state.nodeTypes;
    edgeTypes = state.edgeTypes;
    for(int i=0; i<graph->getSize(); i++) {
        emit nodeStateChange(nodeTypes[i], graph->getId(i));
        for(int j=0; j<graph->getAdjNum(i); j++) {
            int adj_in_nodes = graph->getAdjIndexInNodes(i,j);
            emit edgeStateChange(edgeTypes[i][adj_in_nodes], graph->getId(i), graph->getId(adj_in_nodes));
        }
    }

    int aiiu;
    int from_u;
    if (chosenAlgo != Melysegi) {
        if (u != -1) emit nodeStateChange(NodeType::ExamineAdj, graph->getId(u));
        aiiu = adj_ind_in_u;
        from_u = u;
    } else {
        if (u != -1 && ((adj_ind_in_us[u] == 0 && graph->getAdjNum(u) == 0 && sig == First) || sig == Last)) emit nodeStateChange(NodeType::ExamineAdj, graph->getId(u));
        else if (u == -1 && sig == Last) ;
        else if (prev_u != -1) emit nodeStateChange(NodeType::ExamineAdj, graph->getId(prev_u));
        else if (u != -1) emit nodeStateChange(NodeType::ExamineAdj, graph->getId(u));

        aiiu = (prev_u == -1) || (sig == First) || sig == Last ? -1 : adj_ind_in_us[prev_u];
        from_u = prev_u;
    }
    if ((aiiu-1) != -1 && aiiu != -1) {
        int adj_index = graph->getAdjIndexInNodes(from_u,(aiiu-1));
        emit nodeStateChange(NodeType::ExaminedNode, graph->getId(adj_index));
        emit edgeStateChange(EdgeType::ExaminedEdge, graph->getId(from_u), graph->getId(adj_index));
    }

    discovery_time = state.discovery_time;
    finishing_time = state.finishing_time;
    r = state.r;
    time = state.time;
    prev_u = state.prev_u;
    in_dfs_visit = state.in_dfs_visit;

    if (steps.length() == 1) return false;
    return true;
}


// EGYÉB SEGÉDFÜGGVÉNYEK

int Algorithm::remMin(QVector<int> &q)
{
    int min_ind = q.at(0);
    int min_d = INT32_MAX;
    foreach(int index, q) {
        if (distances.at(index) < min_d) {
            min_d = distances.at(index);
            min_ind = index;
        }
    }
    q.removeAll(min_ind);
    return min_ind;
}

void Algorithm::addState()
{
    AlgorithmState state = AlgorithmState(distances, parents, queue, u, adj_ind_in_u, nodeTypes, edgeTypes,
                                          discovery_time, finishing_time, adj_ind_in_us, r, time, prev_u, in_dfs_visit,
                                          sig);
    steps.push(state);
}

QString Algorithm::queueToVectorMin()
{
    QString str = "< ";
    QVector<int> q = QVector<int>(queue);
    while (!q.isEmpty()) {
        int min_ind = remMin(q);
        str += QString(graph->getName(graph->getId(min_ind))) + ", ";
    }
    if (str.length() > 4) str.remove(str.length()-2, 2);
    str += " >";
    return str;
}

QString Algorithm::queueToVector()
{
    QString str = "< ";
    QVector<int> q = QVector<int>(queue);
    while (!q.isEmpty()) {
        str += QString(graph->getName(graph->getId(q.first()))) + ", ";
        q.removeFirst();
    }
    if (str.length() > 4) str.remove(str.length()-2,2);
    str += " >";
    return str;
}

