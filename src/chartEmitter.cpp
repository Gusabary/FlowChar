#include "chartEmitter.h"

namespace FC { namespace BE {

ChartEmitter::ChartEmitter(std::shared_ptr<IR::Stm> tree) : tree(tree) {}

void ChartEmitter::attachChartInfo() {
    IR::SeqStm *tree = (IR::SeqStm *)(&*(this->tree));
    tree->Attach();
}
}}