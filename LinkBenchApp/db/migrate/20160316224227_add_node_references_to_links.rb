class AddNodeReferencesToLinks < ActiveRecord::Migration
  def change
	add_reference :links, :node, index: true
  end
end
