using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ClientApplication
{
    public partial class MainWindow : Form
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void MainWindow_Load(object sender, EventArgs e)
        {
            WindowsMediaPlayer.uiMode = "None";
        }

        private void guna2TextBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void guna2Button1_Click(object sender, EventArgs e)
        {
            string fpath = "";
            try
            {
                OpenFileDialog fd = new OpenFileDialog();
                fd.Title = "ALPR Media File";
                fd.Filter = "MP4 Video (*.mp4)|*.mp4|WMV Video (*.wmv)|*.wmv|Quick Movie File (*.mov)|*.mov|All Files (*.*)|*.*";
                DialogResult status = fd.ShowDialog();
                if (status == DialogResult.OK)
                {
                    fpath = fd.FileName;
                    pictureBoxVideo.Hide();
                    WindowsMediaPlayer.URL = fpath;
                    WindowsMediaPlayer.Ctlcontrols.play();
                    pictureBoxPlay.Hide();
                    timer1.Start();
                    pictureBoxPause.Show();
                }
            }
            catch (Exception ex)
            {
                System.Console.WriteLine(ex.Message);
            }
        }

        private void guna2Button2_Click(object sender, EventArgs e)
        {
            
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void guna2HtmlToolTip1_Popup(object sender, PopupEventArgs e)
        {

        }

        private void pictureBoxPause_Click(object sender, EventArgs e)
        {
            pictureBoxPause.Hide();
            pictureBoxPlay.Show();
            WindowsMediaPlayer.Ctlcontrols.pause();
        }

        private void guna2ControlBox2_Click(object sender, EventArgs e)
        {

        }

        private void guna2ControlBox3_Click(object sender, EventArgs e)
        {

        }

        private void pictureBoxPlay_Click(object sender, EventArgs e)
        {
            if(WindowsMediaPlayer.playState == WMPLib.WMPPlayState.wmppsStopped)
            {
                pictureBoxVideo.Hide();
                WindowsMediaPlayer.Ctlcontrols.play();
                pictureBoxPlay.Hide();
                timer1.Start();
                pictureBoxPause.Show();
            }
            else if(WindowsMediaPlayer.playState == WMPLib.WMPPlayState.wmppsPaused)
            {
                pictureBoxPlay.Hide();
                pictureBoxPause.Show();
                WindowsMediaPlayer.Ctlcontrols.play();
            }
        }

        private void axWindowsMediaPlayer_Enter(object sender, EventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if(WindowsMediaPlayer.playState == WMPLib.WMPPlayState.wmppsPlaying)
            {
                guna2TrackBarvideo.Maximum = (int)WindowsMediaPlayer.Ctlcontrols.currentItem.duration;
                guna2TrackBarvideo.Value = (int)WindowsMediaPlayer.Ctlcontrols.currentPosition;
            }
            labelStart.Text = WindowsMediaPlayer.Ctlcontrols.currentPositionString;
            labelEnd.Text = WindowsMediaPlayer.Ctlcontrols.currentItem.durationString.ToString();
            if(guna2TrackBarvideo.Maximum == guna2TrackBarvideo.Value)
            {
                timer1.Stop();
                pictureBoxVideo.Show();
                pictureBoxPause.Hide();
                pictureBoxPlay.Show();
                guna2TrackBarvideo.Value = 0;
                labelStart.Text = "00:00";
                labelEnd.Text = "00:00";
            }
        }

        private void pictureBoxStop_Click(object sender, EventArgs e)
        {
            WindowsMediaPlayer.Ctlcontrols.stop();
            pictureBoxVideo.Show();
            timer1.Stop();
            guna2TrackBarvideo.Value = 0;
            labelStart.Text = "00:00";
            labelEnd.Text = "00:00";
            pictureBoxPause.Hide();
            pictureBoxPlay.Show();
        }

        private void guna2CirclePictureBox2_Click(object sender, EventArgs e)
        {

        }

        private void guna2CirclePictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void guna2Panel3_Paint(object sender, PaintEventArgs e)
        {

        }

        private void tabPage1_Click(object sender, EventArgs e)
        {

        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void label7_Click(object sender, EventArgs e)
        {

        }
    }
}
