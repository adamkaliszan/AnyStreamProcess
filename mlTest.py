#!/usr/bin/env python3

import core.ml.trDistributionML as mld


intensities=[4,4,4,4,4,4,4,4,4,4,4
            ,0,1,2,3,4,5,6,7,8,9,10]


result=mld.calculate(intensities, "./core/ml/trained_models/uniform_model_all_points")

print("Rezultaty obliczeń: {}".format(result))



