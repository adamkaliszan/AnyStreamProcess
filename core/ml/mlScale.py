#!/bin/env python 3
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
from numpy import expand_dims

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

import sklearn

from sklearn import preprocessing

from tensorflow.keras.layers.experimental import preprocessing

np.set_printoptions(precision=3, suppress=True)



#Użycie zapisanego modelu
loaded_model = keras.models.load_model("./trained_models/uniform_model_all_points/")
#model.summary()

def calculateP_baseOnOutIntensities(model, intensities):
    #change input values order accordingly to data schema for which model was trained
    input_intensities = intensities[0::2]
    service_intensities = intensities[1::2]
    intensities2 = input_intensities + service_intensities
    
    intensities2 = np.array(intensities2)
    print(intensities2.shape)
    intensities2 = expand_dims(intensities2, axis=0)
    print(intensities2.shape)
    results = model.predict(intensities2)
    return results[0, :].tolist()



