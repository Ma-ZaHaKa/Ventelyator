using SimpleJSON;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO.Ports;
using System.Reflection;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Project
{
    // Наследуемся от FormShadow
    public partial class Form1 : FormShadow
    {
        public Form1()
        {
            InitializeComponent();

            // Плавное закрытие программы
            async void Exit() { for (Opacity = 1; Opacity > .0; Opacity -= .2) await Task.Delay(7); Close(); }
            ButtonClose.Click += (s, a) => Exit();

            // Красим форму
            FormPaint(Color.FromArgb(44, 57, 67), Color.FromArgb(35, 44, 55));

            // Позволяем таскать за заголовок Label и Panel
            new List<Control> { LabelHead, PanelHead }.ForEach(x =>
            {
                x.MouseDown += (s, a) =>
                {
                    x.Capture = false; Capture = false; Message m = Message.Create(Handle, 0xA1, new IntPtr(2), IntPtr.Zero); base.WndProc(ref m);
                };
            });

            //---double buffered panels by diktor
            new List<Panel> { PanelHead, PanelMain}.ForEach(panel
               => typeof(Panel).InvokeMember("DoubleBuffered", BindingFlags.SetProperty | BindingFlags.Instance | BindingFlags.NonPublic, null, panel, new object[] { true }));
        }

        // Красим форму
        public void FormPaint(Color color1, Color color2)
        {
            void OnPaintEventHandler(object s, PaintEventArgs a)
            {
                if (ClientRectangle == Rectangle.Empty)
                    return;

                var lgb = new LinearGradientBrush(ClientRectangle, Color.Empty, Color.Empty, 90);
                var cblend = new ColorBlend { Colors = new[] { color1, color1, color2, color2 }, Positions = new[] { 0, 0.09f, 0.09f, 1 } };

                lgb.InterpolationColors = cblend;
                a.Graphics.FillRectangle(lgb, ClientRectangle);
            }

            Paint -= OnPaintEventHandler;
            Paint += OnPaintEventHandler;

            Invalidate();
        }

        void FormPaint(Color color_shapka, int pixel_shapka, Color color_body)
        {
            void OnPaintEventHandler(object s, PaintEventArgs a)
            {
                if (ClientRectangle == Rectangle.Empty)
                    return;
                float shapka_panel = (float)pixel_shapka / 600; //0.05f; //telegram   //shapka * 600 = count_pixel on screen

                var lgb = new LinearGradientBrush(ClientRectangle, Color.Empty, Color.Empty, 90);
                var cblend = new ColorBlend { Colors = new[] { color_shapka, color_shapka, color_body, color_body }, Positions = new[] { 0, shapka_panel, shapka_panel, 1 } };

                lgb.InterpolationColors = cblend;
                a.Graphics.FillRectangle(lgb, ClientRectangle);
            }

            Paint -= OnPaintEventHandler;
            Paint += OnPaintEventHandler;

            Invalidate();
        }









        public string COM = "";
        public string ARDUINO_PROJECT = "debug_station";
        async void Form1_Load(object sender, EventArgs e)
        {
            // Плавный запуск формы
            for (Opacity = 0; Opacity < 1; Opacity += .2) await Task.Delay(10);

            #region OLD
            //try  // inital
            //{
            //    int timeout = 10000;
            //   serialPort1.BaudRate = (9600);
            //    serialPort1.ReadTimeout = (timeout);
            //    serialPort1.WriteTimeout = (timeout);
            //    //portbox.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());
            //    //for (int i = 0; i < portbox.Items.Count; i++) { if (portbox.Items[i].ToString().Contains("3")) { portbox.SelectedIndex = i; } }
            //    if (portbox.Items.Count == 0) { throw new Exception("no com"); }
            //    portbox.SelectedIndex = 0;
            //    serialPort1.PortName = portbox.Items[0].ToString();
            //}
            //catch (Exception ex)
            //{
            //    MessageBox.Show("Ардуино подключи");
            //    Close();
            //}
            //try
            //{
            //    System.Threading.Thread.Sleep(500);
            //    string _out = SendData("get_data");
            //    var data = _out.Split(':');
            //    int l_perc = int.Parse(data[1]);
            //    int r_perc = int.Parse(data[3]);
            //    L.Text = $"{l_perc}";
            //    LV.Value = l_perc;
            //    R.Text = $"{r_perc}";
            //    RV.Value = r_perc;
            //}
            //catch
            //{
            //    L.Text = "0";
            //    LV.Value = 0;
            //    R.Text = "0";
            //    RV.Value = 0;
            //}
            #endregion



            for (int i = 0; i < 5; i++) { COM = FindCOMPort(ARDUINO_PROJECT); if (COM == "") { System.Threading.Thread.Sleep(1000); } else { break; } }
            //COM = FindCOMPort(ARDUION_PROJECT);
            if (COM == "") { MessageBox.Show("ERROR! COM NOT FOUND"); this.Close(); }

            List<(string, int)> vals = GetVals(COM);
            int tmp_val = 0;
            tmp_val = vals.Find(p => (p.Item1 == "1")).Item2;

            L.Text = $"{tmp_val}";
            LV.Value = tmp_val;

            tmp_val = vals.Find(p => (p.Item1 == "2")).Item2;
            R.Text = $"{tmp_val}";
            RV.Value = tmp_val;
        }


        void RV_Scroll(object sender, ScrollEventArgs e)
        {
            int perc = (int)RV.Value; //0-100
            R.Text = $"{perc}";
            //LOG.Text = $"{perc}";
            //SendData($"R,{perc}");
            OnChangeUI(COM, "2", $"{perc}");
        }

        void LV_Scroll(object sender, ScrollEventArgs e)
        {
            int perc = (int)LV.Value; //0-100
            L.Text = $"{perc}";
            //LOG.Text = $"{perc}";
            //SendData($"L,{perc}");
            OnChangeUI(COM, "1", $"{perc}");
        }
        //string SendData(string data)
        //{
        //    if (!serialPort1.IsOpen) { serialPort1.Open(); }
        //    serialPort1.WriteLine(data);
        //    string _out = serialPort1.ReadLine();
        //    serialPort1.Close();
        //    return _out;
        //}


        //void portbox_SelectedIndexChanged(object sender, EventArgs e)
        //{
        //    //(System.IO.Ports.SerialPort)serialPort1.PortName = (ComboBox)portbox.Text;
        //    //serialPort1.PortName = portbox.Text;
        //}

        int GetVal(string _port, string _vent)
        {
            int err_code = 0;
            string response = SendMsgCom(_port, GetJsonGetVentVal(_vent), 9600);
            if (response == "") { MessageBox.Show("ERROR GETTING VALUE!"); }
            JSONNode data = JSON.Parse(response);
            if (data["error"].Value != "") { MessageBox.Show($"ERROR! {data["error"].Value}"); return err_code; }
            else { try { return int.Parse(data[0].Value); } catch { return err_code; } }
        }
        List<(string, int)> GetVals(string _port)
        {
            int err_code = 0;
            string response = SendMsgCom(_port, GetJsonGetVentsVal(), 9600);
            if (response == "") { MessageBox.Show("ERROR GETTING VALUE!"); }
            JSONNode data = JSON.Parse(response);
            if (data["error"].Value != "") { MessageBox.Show($"ERROR! {data["error"].Value}"); return new List<(string, int)> { ("~error", err_code) }; }
            else
            {
                try
                {
                    List<(string, int)> _out = new List<(string, int)>();
                    foreach (KeyValuePair<string, JSONNode> kvp in data.AsObject)
                    {
                        if (int.TryParse(kvp.Value.Value, out int value))
                        {
                            _out.Add((kvp.Key, value));
                        }
                    }
                    return _out;
                }
                catch { return new List<(string, int)> { ("~error", err_code) }; }
            }
        }

        void OnChangeUI(string _port, string _vent, string _val)
        {
            //bool status = (SendMsgCom(_port, GetSetValueJSON(_vent, _val), 9600) != "");
            //if (!status) { MessageBox.Show("ERROR SETTING VALUE!"); }
            //SendMsgCom(_port, GetJsonSetVentVal(_vent, _val), 9600);
            SendMsgComNoResp(_port, GetJsonSetVentVal(_vent, _val), 9600);
            //SendMsgComNoRespAsync(_port, GetSetValueJSON(_vent, _val), 9600);
        }
    }
}
