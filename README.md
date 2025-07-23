DJ application project using JUCE: 

I attempted to make a DJ application of sorts using JUCE software. The concept was that it should be more beginner friendly, where the user simply presses buttons like “play the chorus drums from this song simultaneously with the verse vocals from this”.  Along with better automatic tempo matching / beat sync I still believe this could work to produce some genuinely interesting music and I plan to come back to it.

Learnings:

Generally working with JUCE gave me experience working with a high-level framework that abstracts complexity for you, compared to my previous (mostly) standard library experience.
General C++ experience working with manual / automatic memory management, syntax and project organisation and OOP.
Working and creating separate threads / threadpools for different tasks. It was important to separate audio and UI threads with mutex/scoped locks, as well as using background threads for other tasks (such as doing background stretching of songs so as not to make the UI wait). I plan on using this outside of JUCE in future projects too.
Integrating libraries such as SoundTorch and then RubberBand, the latter not having many online resources. Good to get experience with the “admin” side of programming.
Extensively used AI assistance (chatgpt) and further learnt where it is useful and where it falls short. 

Shortcomings:

RubberBand is an open source constant pitch stretching software that I tried to use. The live stretching mode resulted in garbage audio quality so I attempted to use a higher quality offline stretch mode on loading. This was slow but worked. The problem was that my stem separating python script (which also created a “beatgrid” and estimated a bpm using madmom) was not accurate enough with tempo estimation, so two songs I was mixing would drift over time. I tried and failed to implement a correcting sync class which matches the beatgrids to a master clock, but I believe this could still work.

Really this was one (big) step away from a successful project but considering high quality, live, constant pitch stretching algorithms like elastique cost a lot of money and require licensing rights, I’m not too disappointed that I was unable to replicate it.

Time spent ~ 2 weeks 

