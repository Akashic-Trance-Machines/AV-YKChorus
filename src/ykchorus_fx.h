//
// ykchorus_fx.h
//
// AV-YKChorus — IAudioFX wrapper for TAL YKChorus (Juno-60 style chorus).
// Based on TAL-NoiseMaker by Patrick Kunz; ported via SpotlightKid/ykchorus.
//
// Copyright (C) 2026  The Akashic Trance Machines Team
// This file is part of AV-YKChorus and is licensed under GPL-3.0.
// See ../LICENSE.
//
#pragma once

#include "engine/iaudiofx.h"

// Forward-declare to avoid including ChorusEngine in every TU.
class ChorusEngine;

class CYKChorusFX : public IAudioFX
{
public:
	CYKChorusFX ();
	~CYKChorusFX () override;

	// IModule
	const char	*Id ()   const override { return "ykchorus"; }
	const char	*Name () const override { return "YKChorus"; }

	void	Init   (unsigned nSampleRate, unsigned nMaxBlock) override;
	void	Reset  () override;

	unsigned		 NumParams ()              const override;
	const TParamDesc	&ParamDesc (unsigned idx)  const override;
	TParamValue		 GetParam  (unsigned idx)  const override;
	void			 SetParam  (unsigned idx, TParamValue v) override;
	int			 FindParam (const char *pId) const override;

	size_t	Serialize   (uint8_t *pBuf, size_t nCap)  const override;
	size_t	Deserialize (const uint8_t *pBuf, size_t nLen)   override;

	// IAudioFX
	void     Process    (float *pIoL, float *pIoR, unsigned nFrames) override;
	unsigned TailFrames () const override { return 0; }	// no tail

private:
	ChorusEngine *m_pEngine;	// heap-allocated in Init()
	float         m_fMix;		// 0.0–1.0
};
