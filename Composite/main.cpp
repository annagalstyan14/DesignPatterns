#include "Panel.h"
#include "Button.h"
#include "Label.h"

int main() {
	Panel* mainWindow = new Panel("Main window");
	Panel* Header = new Panel("Header");
	Panel* Body = new Panel("Body");

	Header->add(new Label("App title"));
	Header->add(new Button("Exit"));

	Body->add(new Label("Welcome"));
	Body->add(new Button("Settings"));
	Body->add(new Button("Start"));

	mainWindow->add(Header);
	mainWindow->add(Body);

	mainWindow->draw();

	delete mainWindow;
	return 0;
}