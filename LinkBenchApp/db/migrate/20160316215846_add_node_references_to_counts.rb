class AddNodeReferencesToCounts < ActiveRecord::Migration
  def change
	add_reference :counts, :node, index: true
  end
end
