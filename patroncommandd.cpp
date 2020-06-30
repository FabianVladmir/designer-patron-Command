#include <stack>
#include <iostream>
#include <memory>
using namespace std;

class ICommand {
  public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
};

/***    ***/
class Televisor {
  bool estado;
  int canal;

public:
  Televisor() {}
  void TeleEncendida()   { estado = true;  }
  void TeleApagada()  { estado = false; }
  
  void CambiarCanal(int channel) {
    canal = channel; 
  }
  
  bool Encendido()       { return estado;}
  int getChannel()  { return canal; }
};

class TvEncendida : public ICommand {
  Televisor *tv;

public:
  TvEncendida(Televisor &Televisor):tv(&Televisor) {}
  void execute()    { tv->TeleEncendida(); }
  void undo()       { tv->TeleApagada();}
  void redo()       { tv->TeleEncendida(); }
};

class TvApagada : public ICommand {
  TvEncendida TvOn;              // command complementario

public:
  TvApagada(Televisor &Televisor):TvOn(Televisor) {}
  void execute()    { TvOn.undo(); }
  void undo()       { TvOn.execute(); }
  void redo()       { TvOn.undo(); }
};

class ControlRemeto : public ICommand {
  Televisor *tv;
  int AntiguoCanal;
  int newCanal;

public:
  ControlRemeto(Televisor *Televisor, int channel) {
    tv = Televisor;
    newCanal = channel;
  }

  void execute() {
    AntiguoCanal = tv->getChannel();
    tv->CambiarCanal(newCanal); 
  }

  void undo() { 
    tv->CambiarCanal(AntiguoCanal);
  }

  void redo() { 
    tv->CambiarCanal(newCanal);
  }
};

typedef stack<shared_ptr<ICommand> > CRemoto;

class ControRemoto {
  CRemoto Sundo;
  CRemoto Sredo;

public:
  ControRemoto() {}

  void executeCmd(shared_ptr<ICommand> command) {
    Sredo = CRemoto(); // limpiar pila
    command->execute();
    Sundo.push(command);
  }

  void undo() {
    if (Sundo.size() <= 0) {
        return;
    }
    Sundo.top()->undo();          // command mas reciente
    Sredo.push(Sundo.top());      // command undo deshacer la pila
    Sundo.pop();                  // eliminar pila superior undo
  }
  
  void redo() {
    if (Sredo.size() <= 0) {
        return;
    }
    Sredo.top()->redo();          // comand mas reciente
    Sundo.push(Sredo.top());       // comand redo rehacer la pila
    Sredo.pop();                  // elimina pila superior redo
  }
};

int main() {
    using namespace std;

    Televisor tv;
    ControRemoto ControRemoto;

    tv.TeleEncendida();
     cout << "Televisor encendido?: " << tv.Encendido() << endl;

    shared_ptr<ICommand> c1(new ControlRemeto(&tv, 1)); // comando canal 1
    ControRemoto.executeCmd(c1);
    cout << "Cambiando a canal :" << tv.getChannel() << endl;
    
    shared_ptr<ICommand> c2(new ControlRemeto(&tv, 2)); // comando canal 2
    ControRemoto.executeCmd(c2);
    cout << "Cambiando a canal :" << tv.getChannel() << endl;
    
    shared_ptr<ICommand> c3(new ControlRemeto(&tv, 3)); // comando canal 3
    ControRemoto.executeCmd(c3);
    cout << "Cambiando a canal :" << tv.getChannel() << endl;
    
    cout << "deshaciendo" << endl;
    ControRemoto.undo();
    cout << "Canal actual: " << tv.getChannel() << endl;
    
    cout << "deshaciendo" << endl;
    ControRemoto.undo();
    cout << "Canal actual: " << tv.getChannel() << endl;
    
    cout << "rehaciendo" << endl;
    ControRemoto.redo();
    cout << "Canal actual: " << tv.getChannel() << endl;
    
    cout << "rehaciendo" << endl;
    ControRemoto.redo();
    cout << "Canal actual: " << tv.getChannel() << endl;

    tv.TeleApagada();
    cout << "Televisor encendido?: " << tv.Encendido() << endl;

  return 0;
}