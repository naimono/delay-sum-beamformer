# DelaySum
This is a repository for an audio plugin that simulates delay sum beamformer. The delays are computed using interaural time difference (ITD) and interaural level difference (ILD) calculations. The two calculations are used since ITD performs well on lower frequencies, whereas ILD performs better on higher frequencies as seen in [Sound source localization](https://www.sciencedirect.com/science/article/pii/S187972961830067X) and others. ITD and ILD calcuations are based on formulas found also in [Sound source localization](https://www.sciencedirect.com/science/article/pii/S187972961830067X). This repository is dependent on [IPlug2](https://github.com/iPlug2/iPlug2) framework and its dependencies.

## Interaural Time Difference (ITD)
![ITD mode](/resources/img/plugin_image_ITD.png)

The image above shows the plugin simulating beamformer with ITD. The left most option shows that the ITD option is selected. The distance between mics and the angle of the sound source with respect to the mics can be manipulated.

## Interaural Level Difference (ILD)
![ITD mode](/resources/img/plugin_image_ILD.png)

The image above shows the plugin simulating beamformer with ILD. The left most option shows that the ILD option is selected. The distance between mics and the angle of the sound source with respect to the mics can be manipulated similarly to the ITD option. For ILD calculation, frequency to focus on is also used, and it can be manipulated using the right most knob.

## Updates
Updated README and fixed ILD implementations on 11/12/2020.
