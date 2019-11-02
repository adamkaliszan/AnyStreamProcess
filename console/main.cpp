#include<QDebug>
#include<QVector>
#include<QList>
#include <iostream>

#include "../core/model.h"

ModelSystem prepareSystem()
{
    QVector<ModelTrClass> trClasses;

    QList<ModelSubResourcess> serverGroups;
    ResourcessScheduler serverScheduler = ResourcessScheduler::Random;
    ModelResourcess resServer( serverGroups, serverScheduler);

    QList<ModelSubResourcess> bufferGroups;
    ResourcessScheduler bufferScheduler = ResourcessScheduler::Random;
    ModelResourcess resBuffer(bufferGroups, bufferScheduler);

    SystemPolicy policy = SystemPolicy::NoBuffer;

    return ModelSystem(trClasses, resServer, resBuffer, policy);
}

int main(int argc, char *argv[])
{
    ModelSystem model = prepareSystem();

    std::cout<<"To jest test";
    return 0;
}
