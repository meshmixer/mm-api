namespace examples
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.queriesButton = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.alignButton = new System.Windows.Forms.Button();
            this.outputTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.testWriteButton = new System.Windows.Forms.Button();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.setColorButton = new System.Windows.Forms.Button();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(7, 6);
            this.button1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(131, 39);
            this.button1.TabIndex = 0;
            this.button1.Text = "planeCut";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(7, 49);
            this.button2.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(131, 41);
            this.button2.TabIndex = 1;
            this.button2.Text = "remesh";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // queriesButton
            // 
            this.queriesButton.Location = new System.Drawing.Point(297, 6);
            this.queriesButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.queriesButton.Name = "queriesButton";
            this.queriesButton.Size = new System.Drawing.Size(131, 41);
            this.queriesButton.TabIndex = 2;
            this.queriesButton.Text = "queries";
            this.queriesButton.UseVisualStyleBackColor = true;
            this.queriesButton.Click += new System.EventHandler(this.queriesButton_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(16, 15);
            this.tabControl1.Margin = new System.Windows.Forms.Padding(4);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(945, 363);
            this.tabControl1.TabIndex = 3;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.setColorButton);
            this.tabPage1.Controls.Add(this.alignButton);
            this.tabPage1.Controls.Add(this.outputTextBox);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Controls.Add(this.button1);
            this.tabPage1.Controls.Add(this.queriesButton);
            this.tabPage1.Controls.Add(this.button2);
            this.tabPage1.Location = new System.Drawing.Point(4, 25);
            this.tabPage1.Margin = new System.Windows.Forms.Padding(4);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(4);
            this.tabPage1.Size = new System.Drawing.Size(937, 334);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Basic Stuff";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // alignButton
            // 
            this.alignButton.Location = new System.Drawing.Point(7, 135);
            this.alignButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.alignButton.Name = "alignButton";
            this.alignButton.Size = new System.Drawing.Size(131, 39);
            this.alignButton.TabIndex = 5;
            this.alignButton.Text = "align";
            this.alignButton.UseVisualStyleBackColor = true;
            this.alignButton.Click += new System.EventHandler(this.alignButton_Click);
            // 
            // outputTextBox
            // 
            this.outputTextBox.Location = new System.Drawing.Point(0, 286);
            this.outputTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.outputTextBox.Name = "outputTextBox";
            this.outputTextBox.Size = new System.Drawing.Size(929, 22);
            this.outputTextBox.TabIndex = 4;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(0, 265);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(106, 17);
            this.label1.TabIndex = 3;
            this.label1.Text = "Last Command:";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.testWriteButton);
            this.tabPage2.Location = new System.Drawing.Point(4, 25);
            this.tabPage2.Margin = new System.Windows.Forms.Padding(4);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(4);
            this.tabPage2.Size = new System.Drawing.Size(937, 334);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "LiveMesh";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // testWriteButton
            // 
            this.testWriteButton.Location = new System.Drawing.Point(8, 39);
            this.testWriteButton.Margin = new System.Windows.Forms.Padding(4);
            this.testWriteButton.Name = "testWriteButton";
            this.testWriteButton.Size = new System.Drawing.Size(147, 39);
            this.testWriteButton.TabIndex = 4;
            this.testWriteButton.Text = "Test Write";
            this.testWriteButton.UseVisualStyleBackColor = true;
            // 
            // setColorButton
            // 
            this.setColorButton.Location = new System.Drawing.Point(297, 51);
            this.setColorButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.setColorButton.Name = "setColorButton";
            this.setColorButton.Size = new System.Drawing.Size(131, 39);
            this.setColorButton.TabIndex = 6;
            this.setColorButton.Text = "set mesh color";
            this.setColorButton.UseVisualStyleBackColor = true;
            this.setColorButton.Click += new System.EventHandler(this.setColorButton_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(977, 389);
            this.Controls.Add(this.tabControl1);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "Form1";
            this.Text = "Form1";
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button queriesButton;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Button testWriteButton;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.TextBox outputTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button alignButton;
        private System.Windows.Forms.Button setColorButton;
    }
}

