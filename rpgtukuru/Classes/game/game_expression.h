#pragma once

#include <deque>


namespace GameExpression
{
	// copy from Project.cpp
	float experience(float level, float basic, float increase, float correction);

	float normalAttack(
		float powerA, float defenceB,
		bool critical, bool commandDefend, bool strongDefence
	);
	float normalAttackHitRate(
		float const basicHitRate, float const speedA, float const speedB,
		bool const ignoreDodgeRate,
		float const conditionChange,
		bool const itemDodgeRateUp,
		bool const conditionNoAction
	);
	float skillEffect(
		float const basicEffect,
		float const powerA, float const powerB, float const blowRelate,
		float const  mindA, float const  mindB, float const mindRelate,
		float const spread,
		bool const ignoreDefence, bool const commandDefend, bool const strongDefence
	);
	float skillSuccessRate(
		float const basicSuccessRate, float const conditionChangeRate
	);
	float escapeSuccessRate(
		std::deque<int> const& enemySpeed, std::deque<int> const& charaSpeed,
		float const escapeFailureTime, bool const firstStrike
	);
}; // namespaces GameExpression
