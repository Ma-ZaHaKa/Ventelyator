using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.IO.Ports;
using System.Reflection;
using System.Threading.Tasks;

namespace Project
{
    partial class Form1
    {
        class MyObject
        {
            [JsonProperty("string")]
            public string MyString { get; set; }
        }
        //------------------!!!!!!!!!!!!!! OLD VERSION API ONLY FOR VENT, ACTUAL IN MEMAPI HANDLER
        static string GetJsonHello()
        {
            var jsonObject = new
            {
                mode = "hello",
                //offsets = new[] { "0x2C0" },
            };

            string json = JsonConvert.SerializeObject(jsonObject, Formatting.Indented);
            return json;
        }

        //------------------VENT--------------------------------------
        static string GetJsonSetVentVal(string _vent, string _val)
        {
            var jsonObject = new
            {
                mode = "set_val",
                vent = $"{_vent}",
                value = $"{_val}",
                //offsets = new[] { "0x2C0" },
            };

            string json = JsonConvert.SerializeObject(jsonObject, Formatting.Indented);
            return json;
        }
        static string GetJsonGetVentsVal()
        {
            var jsonObject = new
            {
                mode = "get_val",
                //offsets = new[] { "0x2C0" },
            };

            string json = JsonConvert.SerializeObject(jsonObject, Formatting.Indented);
            return json;
        }

        static string GetJsonGetVentVal(string _vent)
        {
            var jsonObject = new
            {
                mode = "get_val",
                vent = $"{_vent}",
                //offsets = new[] { "0x2C0" },
            };

            string json = JsonConvert.SerializeObject(jsonObject, Formatting.Indented);
            return json;
        }
        //------------------------------------------------------------






        //-------------------------------------------------OTHER-------
        static string GetJsonSetGlobalValue(int value, int disp_num = 0, bool backlight = true)
        {

            var jsonObjectb1 = new
            {
                mode = "set_glob_val",
                value = $"{value}",
                disp_num = $"{disp_num}",
                backlight = $"1",
                //offsets = new[] { "0x2C0" },
            };

            var jsonObjectb0 = new
            {
                mode = "set_glob_val",
                value = $"{value}",
                disp_num = $"{disp_num}",
                //offsets = new[] { "0x2C0" },
            };

            string jsonb1 = JsonConvert.SerializeObject(jsonObjectb1, Formatting.Indented);
            string jsonb0 = JsonConvert.SerializeObject(jsonObjectb0, Formatting.Indented);
            return backlight ? jsonb1 : jsonb0;
        }
        static string GetJsonSetGlobalValue(string value, int disp_num = 0, bool backlight = true)
        {

            var jsonObjectb1 = new
            {
                mode = "set_glob_val",
                value = $"{value}",
                disp_num = $"{disp_num}",
                backlight = $"1",
                //offsets = new[] { "0x2C0" },
            };

            var jsonObjectb0 = new
            {
                mode = "set_glob_val",
                value = $"{value}",
                disp_num = $"{disp_num}",
                //offsets = new[] { "0x2C0" },
            };

            string jsonb1 = JsonConvert.SerializeObject(jsonObjectb1, Formatting.Indented);
            string jsonb0 = JsonConvert.SerializeObject(jsonObjectb0, Formatting.Indented);
            return backlight ? jsonb1 : jsonb0;
        }








        /*static string GetSetValueJSON(string _value)
        {
            var jsonObject = new
            {
                mode = "print",
                value = _value,
                //offsets = new[] { "0x2C0" },
            };

            string json = JsonConvert.SerializeObject(jsonObject, Formatting.Indented);
            return json;
        }
        static string GetSetValueWSTRJSON(string _descr, string _value)
        {
            var jsonObject = new
            {
                mode = "print_wstr",
                descr = _descr,
                value = _value,
                //offsets = new[] { "0x2C0" },
            };

            string json = JsonConvert.SerializeObject(jsonObject, Formatting.Indented);
            return json;
        }
        static string GetSetValueWSTRJSON(string _descr, string _value, int _row)
        {
            var jsonObject = new
            {
                mode = "print_wstr",
                descr = _descr,
                value = _value,
                row = _row,
                //offsets = new[] { "0x2C0" },
            };

            string json = JsonConvert.SerializeObject(jsonObject, Formatting.Indented);
            return json;
        }*/

    }
}
