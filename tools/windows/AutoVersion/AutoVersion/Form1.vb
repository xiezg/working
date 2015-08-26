
Imports Microsoft.VisualStudio.VCProject
Imports Microsoft.VisualStudio.VCProjectEngine

Imports EnvDTE80
Imports EnvDTE

Public Class Form1

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click

        ' Get the main automation object for Visual Studio
        Dim dte As EnvDTE.DTE = CType(Microsoft.VisualBasic.Interaction.CreateObject("VisualStudio.DTE.8.0"), EnvDTE.DTE)

        ' Get a solution object representing the default solution of Visual Studio and save the solution.
        Dim soln As EnvDTE80.Solution2 = CType(dte.Solution, EnvDTE80.Solution2)

        soln.Open("C:\Users\Xiezg\Desktop\lala\lala.sln")

        Dim prjs As Projects = soln.Projects
        Dim prj As Project = prjs.Item(1)

        MsgBox(prj.Kind)

        prj = prjs.Item(2)
        MsgBox(prj.Kind)

        prj.Delete()

        'MsgBox(prj.ExtenderCATID())
        'MsgBox(prj.FullName)
        'MsgBox(prj.Name)


        soln.Close()

    End Sub

    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click

        Dim Engine As VCProjectEngine
        Dim Proj As VCProject

        Engine = New VCProjectEngineObject()
        Proj = Engine.LoadProject("C:\Users\Xiezg\Desktop\lala\lala\lala.vcproj")



    End Sub
End Class
