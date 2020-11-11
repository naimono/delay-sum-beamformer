# DelaySum
This is a repository for an audio plugin that simulates delay sum beamformer. The delays are computed using interaural time difference (ITD) and interaural level difference (ILD) calculations. The two calculations are used since ITD performs well on lower frequencies, whereas ILD performs better on higher frequencies as seen in [Sound source localization](https://www.sciencedirect.com/science/article/pii/S187972961830067X) and others. ITD and ILD calcuations are based on formulas found also in [Sound source localization](https://www.sciencedirect.com/science/article/pii/S187972961830067X). This repository is dependent on [IPlug2](https://github.com/iPlug2/iPlug2) framework and its dependencies.

## Delay Sum Beamformer


## Interaural Time Difference (ITD)
ITD = (c/v) * (&alpha * sin(&alpha))

## Interaural Level Difference (ILD)
ILD = 0.18 * (f * sin(&alpha))^(1/2)

## Updates
Updated README and fixed ILD implementations on 
