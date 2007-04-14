program HowToUse_Dll;

uses
  Forms,
  HowToUse_Dll in 'HowToUse_Dll.pas' {Form1},
  MediaInfoDll in 'MediaInfoDll.pas';

{$R HowToUse_Dll.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
