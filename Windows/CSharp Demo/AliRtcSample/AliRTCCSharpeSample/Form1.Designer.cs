namespace AliRtcSample
{
    partial class AliRtcSample
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox_LocalView = new System.Windows.Forms.GroupBox();
            this.panel_LocalView = new System.Windows.Forms.Panel();
            this.textBox_UserName = new System.Windows.Forms.TextBox();
            this.label_ChannelNum = new System.Windows.Forms.Label();
            this.textBox_ChannelNum = new System.Windows.Forms.TextBox();
            this.button_Join = new System.Windows.Forms.Button();
            this.button_Leave = new System.Windows.Forms.Button();
            this.listBox_Tips = new System.Windows.Forms.ListBox();
            this.labelUserName = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.check_autoPush = new System.Windows.Forms.CheckBox();
            this.checkAudio = new System.Windows.Forms.CheckBox();
            this.checkCamera = new System.Windows.Forms.CheckBox();
            this.checkAutoSub = new System.Windows.Forms.CheckBox();
            this.checkH5 = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.checkScreen = new System.Windows.Forms.CheckBox();
            this.btn_left = new System.Windows.Forms.Button();
            this.btn_right = new System.Windows.Forms.Button();
            this.groupBox_LocalView.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_LocalView
            // 
            this.groupBox_LocalView.Controls.Add(this.panel_LocalView);
            this.groupBox_LocalView.Location = new System.Drawing.Point(1118, 23);
            this.groupBox_LocalView.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox_LocalView.Name = "groupBox_LocalView";
            this.groupBox_LocalView.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox_LocalView.Size = new System.Drawing.Size(235, 189);
            this.groupBox_LocalView.TabIndex = 1;
            this.groupBox_LocalView.TabStop = false;
            this.groupBox_LocalView.Text = "Local View";
            // 
            // panel_LocalView
            // 
            this.panel_LocalView.BackColor = System.Drawing.SystemColors.ControlDark;
            this.panel_LocalView.Location = new System.Drawing.Point(10, 32);
            this.panel_LocalView.Margin = new System.Windows.Forms.Padding(4);
            this.panel_LocalView.Name = "panel_LocalView";
            this.panel_LocalView.Size = new System.Drawing.Size(217, 148);
            this.panel_LocalView.TabIndex = 0;
            // 
            // textBox_UserName
            // 
            this.textBox_UserName.Location = new System.Drawing.Point(124, 721);
            this.textBox_UserName.Margin = new System.Windows.Forms.Padding(4);
            this.textBox_UserName.Name = "textBox_UserName";
            this.textBox_UserName.Size = new System.Drawing.Size(189, 28);
            this.textBox_UserName.TabIndex = 3;
            // 
            // label_ChannelNum
            // 
            this.label_ChannelNum.AutoSize = true;
            this.label_ChannelNum.Location = new System.Drawing.Point(18, 686);
            this.label_ChannelNum.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label_ChannelNum.Name = "label_ChannelNum";
            this.label_ChannelNum.Size = new System.Drawing.Size(89, 18);
            this.label_ChannelNum.TabIndex = 4;
            this.label_ChannelNum.Text = "Channel :";
            // 
            // textBox_ChannelNum
            // 
            this.textBox_ChannelNum.Location = new System.Drawing.Point(124, 683);
            this.textBox_ChannelNum.Margin = new System.Windows.Forms.Padding(4);
            this.textBox_ChannelNum.Name = "textBox_ChannelNum";
            this.textBox_ChannelNum.Size = new System.Drawing.Size(189, 28);
            this.textBox_ChannelNum.TabIndex = 5;
            // 
            // button_Join
            // 
            this.button_Join.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.button_Join.Location = new System.Drawing.Point(987, 670);
            this.button_Join.Margin = new System.Windows.Forms.Padding(4);
            this.button_Join.Name = "button_Join";
            this.button_Join.Size = new System.Drawing.Size(105, 45);
            this.button_Join.TabIndex = 6;
            this.button_Join.Text = "Join";
            this.button_Join.UseVisualStyleBackColor = true;
            this.button_Join.Click += new System.EventHandler(this.button_Join_Click);
            // 
            // button_Leave
            // 
            this.button_Leave.Location = new System.Drawing.Point(987, 711);
            this.button_Leave.Margin = new System.Windows.Forms.Padding(4);
            this.button_Leave.Name = "button_Leave";
            this.button_Leave.Size = new System.Drawing.Size(105, 45);
            this.button_Leave.TabIndex = 7;
            this.button_Leave.Text = "Leave";
            this.button_Leave.UseVisualStyleBackColor = true;
            this.button_Leave.Click += new System.EventHandler(this.button_Leave_Click);
            // 
            // listBox_Tips
            // 
            this.listBox_Tips.FormattingEnabled = true;
            this.listBox_Tips.ItemHeight = 18;
            this.listBox_Tips.Location = new System.Drawing.Point(1118, 624);
            this.listBox_Tips.Margin = new System.Windows.Forms.Padding(4);
            this.listBox_Tips.Name = "listBox_Tips";
            this.listBox_Tips.ScrollAlwaysVisible = true;
            this.listBox_Tips.Size = new System.Drawing.Size(259, 130);
            this.listBox_Tips.TabIndex = 8;
            // 
            // labelUserName
            // 
            this.labelUserName.AutoSize = true;
            this.labelUserName.Location = new System.Drawing.Point(13, 724);
            this.labelUserName.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelUserName.Name = "labelUserName";
            this.labelUserName.Size = new System.Drawing.Size(98, 18);
            this.labelUserName.TabIndex = 9;
            this.labelUserName.Text = "User Name:";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.richTextBox1);
            this.groupBox1.Location = new System.Drawing.Point(335, 656);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(280, 98);
            this.groupBox1.TabIndex = 10;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "AppServer";
            // 
            // richTextBox1
            // 
            this.richTextBox1.Location = new System.Drawing.Point(6, 27);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(268, 67);
            this.richTextBox1.TabIndex = 1;
            this.richTextBox1.Text = "";
            // 
            // check_autoPush
            // 
            this.check_autoPush.AutoSize = true;
            this.check_autoPush.Location = new System.Drawing.Point(20, 14);
            this.check_autoPush.Name = "check_autoPush";
            this.check_autoPush.Size = new System.Drawing.Size(106, 22);
            this.check_autoPush.TabIndex = 11;
            this.check_autoPush.Text = "自动推流";
            this.check_autoPush.UseVisualStyleBackColor = true;
            // 
            // checkAudio
            // 
            this.checkAudio.AutoSize = true;
            this.checkAudio.Location = new System.Drawing.Point(10, 14);
            this.checkAudio.Name = "checkAudio";
            this.checkAudio.Size = new System.Drawing.Size(88, 22);
            this.checkAudio.TabIndex = 12;
            this.checkAudio.Text = "音频流";
            this.checkAudio.UseVisualStyleBackColor = true;
            this.checkAudio.CheckedChanged += new System.EventHandler(this.checkAudio_CheckedChanged);
            // 
            // checkCamera
            // 
            this.checkCamera.AutoSize = true;
            this.checkCamera.Location = new System.Drawing.Point(10, 42);
            this.checkCamera.Name = "checkCamera";
            this.checkCamera.Size = new System.Drawing.Size(88, 22);
            this.checkCamera.TabIndex = 13;
            this.checkCamera.Text = "相机流";
            this.checkCamera.UseVisualStyleBackColor = true;
            this.checkCamera.CheckedChanged += new System.EventHandler(this.checkCamera_CheckedChanged);
            // 
            // checkAutoSub
            // 
            this.checkAutoSub.AutoSize = true;
            this.checkAutoSub.Location = new System.Drawing.Point(20, 42);
            this.checkAutoSub.Name = "checkAutoSub";
            this.checkAutoSub.Size = new System.Drawing.Size(106, 22);
            this.checkAutoSub.TabIndex = 14;
            this.checkAutoSub.Text = "自动订阅";
            this.checkAutoSub.UseVisualStyleBackColor = true;
            // 
            // checkH5
            // 
            this.checkH5.AutoSize = true;
            this.checkH5.Location = new System.Drawing.Point(20, 70);
            this.checkH5.Name = "checkH5";
            this.checkH5.Size = new System.Drawing.Size(88, 22);
            this.checkH5.TabIndex = 15;
            this.checkH5.Text = "兼容H5";
            this.checkH5.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.checkAutoSub);
            this.groupBox2.Controls.Add(this.check_autoPush);
            this.groupBox2.Controls.Add(this.checkH5);
            this.groupBox2.Location = new System.Drawing.Point(621, 658);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(116, 98);
            this.groupBox2.TabIndex = 18;
            this.groupBox2.TabStop = false;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.checkScreen);
            this.groupBox3.Controls.Add(this.checkCamera);
            this.groupBox3.Controls.Add(this.checkAudio);
            this.groupBox3.Location = new System.Drawing.Point(743, 658);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(122, 98);
            this.groupBox3.TabIndex = 19;
            this.groupBox3.TabStop = false;
            // 
            // checkScreen
            // 
            this.checkScreen.AutoSize = true;
            this.checkScreen.Location = new System.Drawing.Point(10, 70);
            this.checkScreen.Name = "checkScreen";
            this.checkScreen.Size = new System.Drawing.Size(88, 22);
            this.checkScreen.TabIndex = 14;
            this.checkScreen.Text = "屏幕流";
            this.checkScreen.UseVisualStyleBackColor = true;
            this.checkScreen.CheckedChanged += new System.EventHandler(this.checkScreen_CheckedChanged);
            // 
            // btn_left
            // 
            this.btn_left.Location = new System.Drawing.Point(21, 23);
            this.btn_left.Name = "btn_left";
            this.btn_left.Size = new System.Drawing.Size(57, 122);
            this.btn_left.TabIndex = 20;
            this.btn_left.Text = "<<";
            this.btn_left.UseVisualStyleBackColor = true;
            this.btn_left.Click += new System.EventHandler(this.btn_left_Click);
            // 
            // btn_right
            // 
            this.btn_right.Location = new System.Drawing.Point(961, 12);
            this.btn_right.Name = "btn_right";
            this.btn_right.Size = new System.Drawing.Size(57, 122);
            this.btn_right.TabIndex = 21;
            this.btn_right.Text = ">>";
            this.btn_right.UseVisualStyleBackColor = true;
            this.btn_right.Click += new System.EventHandler(this.btn_right_Click);
            // 
            // AliRtcSample
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1401, 788);
            this.Controls.Add(this.btn_right);
            this.Controls.Add(this.btn_left);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.labelUserName);
            this.Controls.Add(this.listBox_Tips);
            this.Controls.Add(this.button_Leave);
            this.Controls.Add(this.button_Join);
            this.Controls.Add(this.textBox_ChannelNum);
            this.Controls.Add(this.label_ChannelNum);
            this.Controls.Add(this.textBox_UserName);
            this.Controls.Add(this.groupBox_LocalView);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "AliRtcSample";
            this.Text = "AliRtcSample";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.AliRtcSample_FormClosing);
            this.Resize += new System.EventHandler(this.AliRtcSample_SizeChanged);
            this.groupBox_LocalView.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.GroupBox groupBox_LocalView;
        private System.Windows.Forms.Panel panel_LocalView;
        private System.Windows.Forms.TextBox textBox_UserName;
        private System.Windows.Forms.Label label_ChannelNum;
        private System.Windows.Forms.TextBox textBox_ChannelNum;
        private System.Windows.Forms.Button button_Join;
        private System.Windows.Forms.Button button_Leave;
        private System.Windows.Forms.ListBox listBox_Tips;
        private System.Windows.Forms.Label labelUserName;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.CheckBox check_autoPush;
        private System.Windows.Forms.CheckBox checkAudio;
        private System.Windows.Forms.CheckBox checkCamera;
        private System.Windows.Forms.CheckBox checkAutoSub;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btn_left;
        private System.Windows.Forms.Button btn_right;
        private System.Windows.Forms.CheckBox checkScreen;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox checkH5;
    }
}

