
Imports Microsoft.VisualStudio.VCProject
Imports Microsoft.VisualStudio.VCProjectEngine

Imports EnvDTE80
Imports EnvDTE


Public Class Form1

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click

        ' Visual Basic
        ' Get the Solution Name and path from the New Project dialog
        Dim solutionName As String = "hehe"
        Dim projectPath As String = "D:\123"
        ' Get the main automation object for Visual Studio
        Dim dte As EnvDTE.DTE

        dte = CType(Microsoft.VisualBasic.Interaction.CreateObject("VisualStudio.DTE.8.0"), EnvDTE.DTE)
        ' Get a solution object representing the default solution of Visual Studio and save the solution.
        Dim soln As EnvDTE80.Solution2 = CType(dte.Solution, EnvDTE80.Solution2)
        If Not System.IO.Directory.Exists(projectPath) Then
            System.IO.Directory.CreateDirectory(projectPath)
        End If

        soln.Open("C:\Users\xiezg\Desktop\123\123.sln")

        Dim prjs As Projects = soln.Projects
        Dim prj As Project

        Dim msg As String = ""

        ' Go through all of the projects in the solution and display
        ' their names and types.
        prjs = dte.Solution.Projects
        For Each prj In prjs
            msg += "NAME: " & prj.Name & " TYPE: " & prj.Kind & vbCr
        Next
        MsgBox(msg)


        soln.SaveAs(projectPath + ".sln")

    End Sub
End Class
