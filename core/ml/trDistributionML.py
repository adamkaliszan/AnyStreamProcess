#!/bin/env python 3
from tensorflow import keras
from numpy import expand_dims

import numpy as np
import os, sys

from sklearn import preprocessing

np.set_printoptions(precision=3, suppress=True)


#Użycie zapisanego modelu
def calculate(intensities, modelFileName):
    try:
        loaded_model = keras.models.load_model(modelFileName)
        print("Załadowano model, Obliczam rozkłady:\n")

        return calculateP_baseOnOutIntensities(loaded_model, intensities)
    except ImportError:
        sys.stderr.write(f"Nie mogę załadować modelu. Jestem w katalogu {os.getcwd()}")
        return None
    except IOError:
        sys.stderr.write(f"Nie mogę załadować modelu. Jestem w katalogu {os.getcwd()}")
        return None    
    except Exception:
        sys.stderr.write("Coś poszło nie tak")
        return None    
        
def calculateP_baseOnOutIntensities(model, intensities):
    
    intensities2 = np.array(intensities)
    #print(intensities2.shape)
    intensities2 = expand_dims(intensities2, axis=0)
    #print(intensities2.shape)
    results = model.predict(intensities2)
    return results[0, :].tolist()
