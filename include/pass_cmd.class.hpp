/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass_cmd.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlalouli <hlalouli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 16:24:32 by hlalouli          #+#    #+#             */
/*   Updated: 2023/02/22 16:42:32 by hlalouli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PASS_CMD_CLASS_HPP
# define PASS_CMD_CLASS_HPP

#include "client.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Pass : public Command
{
private:
    Client *__client;
public:
    Pass(Client *client);
    virtual ~Pass();
    
    virtual void execute(Mediator* mediator);
};

#endif