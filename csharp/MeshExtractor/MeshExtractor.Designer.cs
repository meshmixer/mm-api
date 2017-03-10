namespace MeshExtractor
{
    partial class MeshExtractor
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
            if (disposing && (components != null)) {
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
            this.mesh_to_obj_button = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.statusLabel = new System.Windows.Forms.Label();
            this.bake_unwrap_uvs = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // mesh_to_obj_button
            // 
            this.mesh_to_obj_button.Location = new System.Drawing.Point(12, 12);
            this.mesh_to_obj_button.Name = "mesh_to_obj_button";
            this.mesh_to_obj_button.Size = new System.Drawing.Size(177, 45);
            this.mesh_to_obj_button.TabIndex = 0;
            this.mesh_to_obj_button.Text = "Extract Selected Mesh";
            this.mesh_to_obj_button.UseVisualStyleBackColor = true;
            this.mesh_to_obj_button.Click += new System.EventHandler(this.mesh_to_obj_button_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(298, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(52, 17);
            this.label1.TabIndex = 1;
            this.label1.Text = "Status:";
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(351, 26);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(102, 17);
            this.statusLabel.TabIndex = 2;
            this.statusLabel.Text = "(disconnected)";
            // 
            // bake_unwrap_uvs
            // 
            this.bake_unwrap_uvs.Location = new System.Drawing.Point(12, 117);
            this.bake_unwrap_uvs.Name = "bake_unwrap_uvs";
            this.bake_unwrap_uvs.Size = new System.Drawing.Size(177, 45);
            this.bake_unwrap_uvs.TabIndex = 3;
            this.bake_unwrap_uvs.Text = "Bake Unwrap UVs";
            this.bake_unwrap_uvs.UseVisualStyleBackColor = true;
            this.bake_unwrap_uvs.Click += new System.EventHandler(this.bake_unwrap_uvs_Click);
            // 
            // MeshExtractor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(618, 443);
            this.Controls.Add(this.bake_unwrap_uvs);
            this.Controls.Add(this.statusLabel);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.mesh_to_obj_button);
            this.Name = "MeshExtractor";
            this.Text = "Meshmixer Mesh Utility";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button mesh_to_obj_button;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.Button bake_unwrap_uvs;
    }
}

