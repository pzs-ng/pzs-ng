unit HowToUse_Dll;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, MediaInfoDll, TntStdCtrls;

type
  TForm1 = class(TForm)
    Memo1: TMemo;
    procedure FormCreate(Sender: TObject);
  private
    { Déclarations privées }
  public
    { Déclarations publiques }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.FormCreate(Sender: TObject);
var
  Handle: Cardinal;
  To_Display: WideString;
begin
  To_Display := MediaInfo_Option (0, 'Info_Version', '');

  To_Display := To_Display + '\r\n\r\nInfo_Parameters\r\n\r\n';
  To_Display := To_Display + MediaInfo_Option (0, 'Info_Parameters', '');

  To_Display := To_Display + '\r\n\r\nInfo_Capacities\r\n';
  To_Display := To_Display + MediaInfo_Option (0, 'Info_Capacities', '');

  To_Display := To_Display + '\r\n\r\nInfo_Codecs\r\n';
  To_Display := To_Display + MediaInfo_Option (0, 'Info_Codecs', '');

  To_Display := To_Display + '\r\n\r\nOpen\r\n';
  Handle := MediaInfo_Open('Example.ogg');

  To_Display := To_Display + '\r\n\r\nInform with Complete=false\r\n';
  MediaInfo_Option (0, 'Complete', '');
  To_Display := To_Display + MediaInfo_Inform(Handle, 0);

  To_Display := To_Display + '\r\n\r\nInform with Complete=true\r\n';
  MediaInfo_Option (0, 'Complete', '1');
  To_Display := To_Display + MediaInfo_Inform(Handle, 0);

  To_Display := To_Display + '\r\n\r\nCustom Inform\r\n';
  MediaInfo_Option (0, 'Inform', 'General;Example : FileSize=%FileSize%');
  To_Display := To_Display + MediaInfo_Inform(Handle, 1);

  To_Display := To_Display + '\r\n\r\nGetI with Stream=General and Parameter:=13\r\n';
  To_Display := To_Display + MediaInfo_GetI(Handle, 0, 0, 13, 1);

  To_Display := To_Display + '\r\n\r\nCount_Get with StreamKind=Stream_Audio\r\n';
  To_Display := To_Display + format('%d', [MediaInfo_Count_Get(Handle, 2, -1)]);

  To_Display := To_Display + '\r\n\r\nGet with Stream:=General and Parameter=^AudioCount^\r\n';
  To_Display := To_Display + MediaInfo_Get(Handle, 0, 0, 'AudioCount', 1, 0);

  To_Display := To_Display + '\r\n\r\nGet with Stream:=Audio and Parameter=^StreamCount^\r\n';
  To_Display := To_Display + MediaInfo_Get(Handle, 2, 0, 'StreamCount', 1, 0);

  To_Display := To_Display + '\r\n\r\nGet with Stream:=General and Parameter=^FileSize^\r\n';
  To_Display := To_Display + MediaInfo_Get(Handle, 0, 0, 'FileSize', 1, 0);

  To_Display := To_Display + '\r\n\r\nClose\r\n';

  MediaInfo_Close(Handle);

  Memo1.Text := To_Display;


end;

end.
