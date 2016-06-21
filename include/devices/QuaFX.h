#ifndef _QUAFX
#define _QUAFX

class Block;
class Quaddon;
class QuasiStack;

class QuaFX
{
public:
						QuaFX(uint32 subt);
						~QuaFX();

	Quaddon				*addonInfo;
	void				*cookie;
};

size_t		ApplyQuaFX(QuasiStack *stack, Block *fxBlock, float **Sig, long nFrames, short nChan);
int			WantedChunks(
					QuasiStack			*stack,
					Block				*fxBlock,
					short				primaryChunk[],
					short				primaryStart[],
					short				primaryEnd[],
					short				primaryClip[],
					int					nPrimary,
					int					maxPrimary
				);

status_t	LoadAddons();


#endif