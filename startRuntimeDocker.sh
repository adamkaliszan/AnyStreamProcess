#!/bin/bash
sudo docker run -v $(pwd):/public/Work -it akaliszan/distribution_calculator:1.0.2 --NoOfSer 12 --NoOfEv 100-- --Ainc 0.1 --Amin 5 --Amax 15 -V 10 --EaDaMin 1 --EaDaMax 3 --asGamma --Output test
