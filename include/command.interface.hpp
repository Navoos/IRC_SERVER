#ifndef COMMAND_INTERFACE_HPP
# define COMMAND_INTERFACE_HPP

class Mediator;

class Command {
  public:
      virtual void execute(Mediator *mediator) = 0;
};
#endif
