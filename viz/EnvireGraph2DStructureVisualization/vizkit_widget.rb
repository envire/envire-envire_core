#
# Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

Vizkit::UiLoader::extend_cplusplus_widget_class "EnvireGraph2DStructureVisualization" do

    #called when the widget is created
    def initialize_vizkit_extension
        #activate Typelib transport via qt slots
        extend Vizkit::QtTypelibExtension
    end

    #called each time vizkit wants to display a new 
    #port with this widget
    def config(value,options)

    end

    #called each time new data are available on the 
    #orocos port connected to the widget the name is
    #custom and can be set via register_widget_for
    def update(sample, port_name)
      displayGraph(sample)
    end
end

# register widget for a specific Typelib type to be 
# accessible via rock tooling (rock-replay,...)
# multiple register_widget_for are allowed for each widget
 Vizkit::UiLoader.register_widget_for("EnvireGraph2DStructureVisualization","/envire/core/EnvireGraph",:update)
 