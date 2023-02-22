/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass_cmd.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlalouli <hlalouli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 18:28:54 by hlalouli          #+#    #+#             */
/*   Updated: 2023/02/22 18:38:32 by hlalouli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pass_cmd.class.hpp"

Pass::Pass(Client *client): __client(client){}

Pass::~Pass(){}

void Pass::execute(Mediator* mediator){
    mediator->pass_cmd(__client);
}


