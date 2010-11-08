#include <kuto/kuto_utility.h>

#include "game_expression.h"


namespace GameExpression
{
	float experience(float const level, float const basic, float const increase, float const correction)
	{
		float result = 0;

		float standard = basic, additional = 1.5f + (increase * 0.01f);
		for(float i = 0; i < (level - 1); i++) {
			result += float(standard);
			standard *= additional;
			additional = (level * 0.002f + 0.8f) * (additional - 1.f) + 1.f;
		}
		result += correction * (level - 1);

		return result;
	}

	float normalAttack(
		float const powerA, float const defenceB,
		bool const commandDefend, bool const strongDefence,
		bool const critical
	) {
		return
			(powerA * 0.5f - defenceB * 0.25f) *
			( kuto::random(0.4f) + 0.8f ) *
			(critical? 3.f : 1.f) *
			( commandDefend? (strongDefence? 0.25f : 0.5f) : 1.f );
	}
	float normalAttackHitRate(
		float const basicHitRate, float const speedA, float const speedB,
		bool const ignoreDodgeRate,
		float const conditionChange,
		bool const itemDodgeRateUp,
		bool const conditionNoAction
	) {
		return
			conditionNoAction? 100.f :
			ignoreDodgeRate? basicHitRate : (
				(
					100.f - (100.f - basicHitRate) *
					( 1.f + ( float(speedB) / float(speedA) - 1.f ) * 0.5f )
				) * conditionChange - (itemDodgeRateUp? 0.25f : 0.f)
			);
	}
	float skillEffect(
		float const basicEffect,
		float const powerA, float const powerB, float const blowRelate,
		float const  mindA, float const  mindB, float const mindRelate,
		float const spread,
		bool const ignoreDefence, bool const commandDefend, bool const strongDefence
	) {
		return
			(
				(
					basicEffect
					+ (powerA * blowRelate * 0.05f )
					+ ( mindA * mindRelate * 0.025f)
				) * ( 1.f + kuto::random(spread * 0.1f) - spread * 0.05f )
				- (
					(powerB * blowRelate * 0.025f ) +
					( mindB * mindRelate * 0.0125f)
				)
			) * ( commandDefend? (strongDefence? 0.25f : 0.5f) : 1.f );
	}
	float skillSuccessRate(
		float const basicSuccessRate, float const conditionChangeRate
	) {
		return basicSuccessRate * conditionChangeRate * 0.00001f;
	}
	float escapeSuccessRate(
		std::deque<int> const& enemySpeed, std::deque<int> const& charaSpeed,
		float const escapeFailureTime, bool const firstStrike
	) {
		if(firstStrike) { return 100.f; }

		float enemyAve = 0;
		for(std::size_t i = 0; i < enemySpeed.size(); i++) { enemyAve += enemySpeed[i]; }
		enemyAve /= enemySpeed.size();

		float charaAve = 0;
		for(std::size_t i = 0; i < charaSpeed.size(); i++) { charaAve += charaSpeed[i]; }
		charaAve /= charaSpeed.size();

		return ( 1.5f - enemyAve / charaAve ) * 100.f - escapeFailureTime * 10.f;
	}
} // namespace GameExpression
