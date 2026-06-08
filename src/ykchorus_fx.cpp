//
// ykchorus_fx.cpp
//
// AV-YKChorus — IAudioFX wrapper for TAL YKChorus.
// Copyright (C) 2026  The Akashic Trance Machines Team
// This file is part of AV-YKChorus and is licensed under GPL-3.0.
// See ../LICENSE.
//
#include "ykchorus_fx.h"
#include "ykchorus/ChorusEngine.h"
#include <cstring>

IModule *Create_ykchorus () { return new CYKChorusFX (); }

// ── Param descriptors ────────────────────────────────────────────────────────

enum ParamId : unsigned
{
	P_MIX = 0,
	P_CHORUS1,
	P_CHORUS2,
	P_RATE1,
	P_RATE2,
	NUM_PARAMS
};

static const TParamDesc s_Params[NUM_PARAMS] =
{
	//  pId         pLabel      Type               Display             fMin  fMax   fDef  fStep  ppOpt nOpt
	{ "mix",       "Mix",      ParamType::Float,  ParamDisplay::Percent, 0,1, 0.5f, 0.01f, nullptr, 0 },
	{ "chorus1",   "Chorus 1", ParamType::Bool,   ParamDisplay::OnOff,   0,1, 1.0f, 1.0f,  nullptr, 0 },
	{ "chorus2",   "Chorus 2", ParamType::Bool,   ParamDisplay::OnOff,   0,1, 1.0f, 1.0f,  nullptr, 0 },
	{ "rate1",     "Rate 1",   ParamType::Float,  ParamDisplay::Percent, 0,1, 0.5f, 0.01f, nullptr, 0 },
	{ "rate2",     "Rate 2",   ParamType::Float,  ParamDisplay::Percent, 0,1, 0.83f,0.01f, nullptr, 0 },
};

static const TParamDesc s_NullParam = { "", "", ParamType::Float, ParamDisplay::Raw, 0,0,0,0, nullptr, 0 };

// ── Constructor / Init ────────────────────────────────────────────────────────

CYKChorusFX::CYKChorusFX ()
:	m_pEngine (nullptr),
	m_fMix (0.5f)
{
}

CYKChorusFX::~CYKChorusFX ()
{
	delete m_pEngine;
}

void CYKChorusFX::Init (unsigned nSampleRate, unsigned /*nMaxBlock*/)
{
	m_pEngine = new ChorusEngine ((float) nSampleRate);
	m_pEngine->setEnablesChorus (true, true);
	m_pEngine->setChorus1LfoRate (0.5f);
	m_pEngine->setChorus2LfoRate (0.83f);
}

void CYKChorusFX::Reset ()
{
	if (m_pEngine)
	{
		delete m_pEngine;
		m_pEngine = nullptr;
		Init (48000, 0);
	}
}

// ── IModule: params ───────────────────────────────────────────────────────────

unsigned CYKChorusFX::NumParams () const { return NUM_PARAMS; }

const TParamDesc &CYKChorusFX::ParamDesc (unsigned idx) const
{
	return (idx < NUM_PARAMS) ? s_Params[idx] : s_NullParam;
}

TParamValue CYKChorusFX::GetParam (unsigned idx) const
{
	if (!m_pEngine) return { 0.0f };
	switch (idx)
	{
	case P_MIX:     return { m_fMix };
	case P_CHORUS1: return { m_pEngine->isChorus1Enabled ? 1.0f : 0.0f };
	case P_CHORUS2: return { m_pEngine->isChorus2Enabled ? 1.0f : 0.0f };
	case P_RATE1:   return { m_pEngine->chorus1L.rate };
	case P_RATE2:   return { m_pEngine->chorus2L.rate };
	default:        return { 0.0f };
	}
}

void CYKChorusFX::SetParam (unsigned idx, TParamValue v)
{
	if (!m_pEngine) return;
	switch (idx)
	{
	case P_MIX:
		m_fMix = v.f < 0.0f ? 0.0f : v.f > 1.0f ? 1.0f : v.f;
		break;
	case P_CHORUS1:
		m_pEngine->setEnablesChorus (v.AsBool (), m_pEngine->isChorus2Enabled);
		break;
	case P_CHORUS2:
		m_pEngine->setEnablesChorus (m_pEngine->isChorus1Enabled, v.AsBool ());
		break;
	case P_RATE1:
		m_pEngine->setChorus1LfoRate (v.f);
		break;
	case P_RATE2:
		m_pEngine->setChorus2LfoRate (v.f);
		break;
	}
}

int CYKChorusFX::FindParam (const char *pId) const
{
	for (unsigned i = 0; i < NUM_PARAMS; i++)
		if (strcmp (s_Params[i].pId, pId) == 0) return (int) i;
	return -1;
}

size_t CYKChorusFX::Serialize   (uint8_t *, size_t) const   { return 0; }
size_t CYKChorusFX::Deserialize (const uint8_t *, size_t)    { return 0; }

// ── IAudioFX: Process ─────────────────────────────────────────────────────────

void CYKChorusFX::Process (float *pIoL, float *pIoR, unsigned nFrames)
{
	if (!m_pEngine) return;
	if (m_fMix == 0.0f) return;

	// Compute dry/wet split matching the Dreamdexed YKChorus wrapper.
	float dry, wet;
	if (m_fMix <= 0.5f)
	{
		dry = 1.0f;
		wet = m_fMix * 2.0f;
	}
	else
	{
		dry = 1.0f - (m_fMix - 0.5f) * 2.0f;
		wet = 1.0f;
	}

	for (unsigned i = 0; i < nFrames; i++)
		m_pEngine->process (dry, wet, &pIoL[i], &pIoR[i]);
}
