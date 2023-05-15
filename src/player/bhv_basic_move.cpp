// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_basic_move.h"

#include "strategy.h"

#include "bhv_basic_tackle.h"

#include "basic_actions/basic_actions.h"
#include "basic_actions/body_go_to_point.h"
#include "basic_actions/body_intercept.h"
#include "basic_actions/neck_turn_to_ball_or_scan.h"
#include "basic_actions/neck_turn_to_low_conf_teammate.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

#include "neck_offensive_intercept_neck.h"

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_BasicMove::execute( PlayerAgent * agent )
{
    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove" );

    //-----------------------------------------------
    // tackle
    if ( Bhv_BasicTackle( 0.8, 80.0 ).execute( agent ) )
    {
        return true;
    }

    const WorldModel & wm = agent->world();
    /*--------------------------------------------------------*/
    // chase ball
    const int self_min = wm.interceptTable().selfStep();//Quantidade de passos para alacançar a bola
    const int mate_min = wm.interceptTable().teammateStep();//Qtd de passos para o colega mais próximo da bola
    const int opp_min = wm.interceptTable().opponentStep();//Qtd de passos para o oponente mais próximo da bola
     
    if ( ! wm.kickableTeammate()//Se existe um jogador para receber um passe (eu acho)
         && ( self_min <= 3
              || ( self_min <= mate_min
                   && self_min < opp_min + 3 )
              )
         )
    {//Intercepta a bola se eu estiver <= 3 passos ou não tiver um oponente com + de 3 passos de proximidade (pra mim faltam 5 pra ele faltam 2) e não tem um colega mais próximo que eu
        dlog.addText( Logger::TEAM,
                      __FILE__": intercept" );
        Body_Intercept().execute( agent );
        agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

        return true;//Finaliza a execução para quando é possível interceptar a bola
    }

    const Vector2D target_point = Strategy::i().getPosition( wm.self().unum() );
    const double dash_power = Strategy::get_normal_dash_power( wm );

    double dist_thr = wm.ball().distFromSelf() * 0.1;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove target=(%.1f %.1f) dist_thr=%.2f",
                  target_point.x, target_point.y,
                  dist_thr );

    agent->debugClient().addMessage( "BasicMove%.0f", dash_power );
    agent->debugClient().setTarget( target_point );
    agent->debugClient().addCircle( target_point, dist_thr );

    if ( ! Body_GoToPoint( target_point, dist_thr, dash_power
                           ).execute( agent ) )
    {
        Body_TurnToBall().execute( agent );
    }

    if ( wm.kickableOpponent()
         && wm.ball().distFromSelf() < 18.0 )
    {
        agent->setNeckAction( new Neck_TurnToBall() );
        
    }
    else
    {
        agent->setNeckAction( new Neck_TurnToBallOrScan( 0 ) );
        return true;
    }

    const PlayerObject *nearest_opp;

    const Vector2D nearest_opp_pos = (nearest_opp ? nearest_opp->pos() : Vector2D(-1000, 0));

    const double nearest_opp_dist = (nearest_opp ? nearest_opp->distFromSelf() : 1000);

    const double closest_teammate_dist = (wm.getDistTeammateNearestTo(nearest_opp_pos, 0));

    if(nearest_opp_dist < 5.0 && nearest_opp_dist < closest_teammate_dist) {
        if (wm.kickableOpponent() == nearest_opp) {
            Vector2D final_point = nearest_opp_pos;
            agent->debugClient().setTarget(final_point);
            
            dlog.addText( Logger::INTERCEPT,
                      __FILE__": no solution... Just go to ball end point (%.2f %.2f)",
                      final_point.x, final_point.y );
            agent->debugClient().addMessage( "InterceptNoSolution" );
            Body_GoToPoint( final_point,
                            2.0,
                            ServerParam::i().maxDashPower()
                            ).execute( agent );
            return true;
        }
    }

    return true;
}
